package main

import (
	"compress/gzip"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"

	fmpb "github.com/formicidae-tracker/studio/bindings/go/fort_myrmidon_pb"
	"github.com/jessevdk/go-flags"
	"google.golang.org/protobuf/encoding/protowire"
	"google.golang.org/protobuf/proto"
)

type Fixer func(*fmpb.FileLine) (bool, error)

type Options struct {
	Verbose bool `short:"v" long:"verbose" description:"verbose mode"`
	DryRun  bool `short:"n" long:"dry-run" description:"Do not modify the file, and just report error"`
}

func ResultName(filename string) string {
	dir := filepath.Dir(filename)
	ext := filepath.Ext(filename)
	baseNoExt := strings.TrimSuffix(filepath.Base(filename), ext)
	return filepath.Join(dir, fmt.Sprintf("%s-fixed%s", baseNoExt, ext))
}

func DecodeMessage(data []byte, m proto.Message) (int, error) {
	messageSize, byteSize := protowire.ConsumeVarint(data)
	if byteSize < 0 {
		return 0, protowire.ParseError(byteSize)
	}
	finalSize := byteSize + int(messageSize)
	return finalSize, proto.Unmarshal(data[byteSize:finalSize], m)
}

func EncodeMessage(m proto.Message, w io.Writer) (int, error) {
	data, err := proto.Marshal(m)
	if err != nil {
		return 0, err
	}
	dataSize := protowire.AppendVarint(nil, uint64(len(data)))
	n, err := w.Write(dataSize)
	if err != nil {
		return n, err
	}
	n, err = w.Write(data)
	if err != nil {
		return len(dataSize) + n, err
	}
	return len(data) + len(dataSize), nil
}

func Execute() error {
	opts := Options{}
	args, err := flags.Parse(&opts)
	if err != nil {
		return err
	}

	if len(args) != 1 {
		return fmt.Errorf("Missing file to fix")
	}

	fixers := []Fixer{FixMeasurement, FixTagCloseUpURI}

	if opts.Verbose == true {
		fixers = append([]Fixer{PrintLine}, fixers...)
	}

	f, err := os.Open(args[0])
	if err != nil {
		return err
	}
	defer f.Close()
	greader, err := gzip.NewReader(f)
	if err != nil {
		return err
	}
	data, err := ioutil.ReadAll(greader)
	if err != nil {
		return err
	}
	f.Close()
	h := fmpb.FileHeader{}
	n, err := DecodeMessage(data, &h)
	if err != nil {
		return err
	}
	data = data[n:]
	log.Printf("Got a myrmidon file version %d.%d", h.MajorVersion, h.MinorVersion)

	lines := []fmpb.FileLine{}
	line := -1
	good := true
	for {
		line += 1
		l := fmpb.FileLine{}
		n, err := DecodeMessage(data, &l)
		if err != nil {
			return err
		}
		data = data[n:]
		keep := true
		for _, f := range fixers {
			keepIt, err := f(&l)
			if err != nil {
				good = false
				log.Printf("Got error on line %d: %s", line, err)
			}
			if keepIt == false {
				keep = false
			}
		}
		if keep == true {
			lines = append(lines, l)
		}

		if len(data) == 0 {
			break
		}
	}

	if opts.DryRun == true {
		if good == false {
			os.Exit(1)
		}
		return nil
	}

	outF, err := os.Create(ResultName(args[0]))
	if err != nil {
		return err
	}
	defer outF.Close()
	gwriter := gzip.NewWriter(outF)
	defer gwriter.Close()

	_, err = EncodeMessage(&h, gwriter)
	if err != nil {
		return err
	}
	for _, l := range lines {
		_, err = EncodeMessage(&l, gwriter)
		if err != nil {
			return err
		}
	}
	return nil
}

func main() {
	if err := Execute(); err != nil {
		log.Fatalf("Unhandled error: %s", err)
	}
}
