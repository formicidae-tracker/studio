package main

import (
	"log"

	fmpb "../../bindings/go/fort_myrmidon_pb"
)

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

func PrintLine(l *fmpb.FileLine) (bool, error) {
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
