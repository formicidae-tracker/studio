package main

import (
	"fmt"
	"path"
	"strconv"
	"strings"

	fmpb "github.com/formicidae-tracker/studio/bindings/go/fort_myrmidon_pb"
)

func FixTagCloseUpURI(l *fmpb.FileLine) (bool, error) {
	if l.Measurement == nil {
		return true, nil
	}

	tagIDStr := path.Base(l.Measurement.TagCloseUpURI)
	if strings.HasPrefix(tagIDStr, "0x") == false {
		tagID, err := strconv.ParseInt(tagIDStr, 10, 32)
		if err != nil {
			return false, err
		}
		err = fmt.Errorf("TagCloseUpURI: '%s' does not use hexadecimal convetion for TagID", l.Measurement.TagCloseUpURI)
		l.Measurement.TagCloseUpURI = path.Join(path.Dir(l.Measurement.TagCloseUpURI),
			fmt.Sprintf("0x%03x", tagID))
		return true, err
	}

	return true, nil

}
