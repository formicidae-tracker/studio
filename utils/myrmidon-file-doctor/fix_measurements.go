package main

import (
	"fmt"
	"math"

	fmpb "../../bindings/go/fort_myrmidon_pb"
)

func FixMeasurement(l *fmpb.FileLine) (bool, error) {
	if l.Measurement == nil {
		return true, nil
	}
	distance := math.Pow(l.Measurement.Start.X-l.Measurement.End.X, 2) + math.Pow(l.Measurement.Start.Y-l.Measurement.End.Y, 2)
	if distance > 1.0 {
		return true, nil
	}
	return false, fmt.Errorf("Invalid measurement of length %.4f pixels (Measurement{%+v}): removing it", math.Sqrt(distance), l.Measurement)
}
