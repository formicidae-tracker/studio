package main

import (
	"compress/gzip"
	"fmt"
	"io/ioutil"
	"log"
	"os"

	fmpb "../../bindings/go/fort_myrmidon_pb"
	"github.com/golang/protobuf/proto"
)

type Fixer func(*fmpb.FileLine, bool) (bool, error)

func PrintExperiment(e *fmpb.Experiment) {
	log.Printf("Experiment{%+v}", e)
}

func PrintAnt(a *fmpb.AntDescription) {
	log.Printf("AntDescription{%+v}", a)

}

func PrintSpace(s *fmpb.Space) {
	log.Printf("Space{%+v}", s)
}

func PrintMeasurement(m *fmpb.Measurement) {
	log.Printf("Measurement{%+v}", m)
}

func PrintLine(l *fmpb.FileLine, dryRun bool) (bool, error) {
	switch {
	case l.AntDescription != nil:
		PrintAnt(l.AntDescription)
	case l.Measurement != nil:
		PrintMeasurement(l.Measurement)
	case l.Space != nil:
		PrintSpace(l.Space)
	case l.Experiment != nil:
		PrintExperiment(l.Experiment)
	}
	return true, nil
}

func Execute() error {
	fixers := []Fixer{PrintLine}

	if len(os.Args) != 2 {
		return fmt.Errorf("I only take one argument, the filepath to the .mrymidon file")
	}
	f, err := os.Open(os.Args[1])
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

	h := fmpb.FileHeader{}
	buffer := proto.NewBuffer(data)
	err = buffer.DecodeMessage(proto.MessageV1(&h))
	if err != nil {
		return err
	}
	log.Printf("Got a myrmidon file version %d.%d", h.MajorVersion, h.MinorVersion)

	lines := []fmpb.FileLine{}
	for {
		l := fmpb.FileLine{}
		err := buffer.DecodeMessage(proto.MessageV1(&l))
		if err != nil {
			return err
		}
		for _, f := range fixers {
			f(&l, true)
		}
		lines = append(lines, l)

		if len(buffer.Unread()) == 0 {
			break
		}
	}

	return nil
}

func main() {
	if err := Execute(); err != nil {
		log.Fatalf("Unhandled error: %s", err)
	}
}
