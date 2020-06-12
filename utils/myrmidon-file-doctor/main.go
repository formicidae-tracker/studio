package main

import (
	"compress/gzip"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"strings"

	fmpb "../../bindings/go/fort_myrmidon_pb"
	"github.com/golang/protobuf/proto"
	"github.com/jessevdk/go-flags"
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

func Execute() error {
	opts := Options{}
	args, err := flags.Parse(&opts)
	if err != nil {
		return err
	}

	if len(args) != 1 {
		return fmt.Errorf("Missing file to fix")
	}

	fixers := []Fixer{}

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
	buffer := proto.NewBuffer(data)
	err = buffer.DecodeMessage(proto.MessageV1(&h))
	if err != nil {
		return err
	}
	log.Printf("Got a myrmidon file version %d.%d", h.MajorVersion, h.MinorVersion)

	lines := []fmpb.FileLine{}
	line := -1
	good := true
	for {
		line += 1
		l := fmpb.FileLine{}
		err := buffer.DecodeMessage(proto.MessageV1(&l))
		if err != nil {
			return err
		}
		keep := true
		for _, f := range fixers {
			keepIt, err := f(&l)
			if err != nil {
				good = false
				log.Printf("Got error on line %d: %s", err)
			}
			if keepIt == false {
				keep = false
			}
		}
		if keep == true {
			lines = append(lines, l)
		}

		if len(buffer.Unread()) == 0 {
			break
		}
	}

	if opts.DryRun == true {
		if good == false {
			os.Exit(1)
		}
		return nil
	}

	outBuffer := proto.NewBuffer(nil)
	err = outBuffer.EncodeMessage(proto.MessageV1(&h))
	if err != nil {
		return err
	}
	for _, l := range lines {
		err = outBuffer.EncodeMessage(proto.MessageV1(&l))
		if err != nil {
			return err
		}
	}
	outF, err := os.Create(ResultName(args[0]))
	if err != nil {
		return err
	}
	defer outF.Close()
	gwriter := gzip.NewWriter(outF)
	defer gwriter.Close()
	_, err = gwriter.Write(outBuffer.Bytes())
	return err
}

func main() {
	if err := Execute(); err != nil {
		log.Fatalf("Unhandled error: %s", err)
	}
}
