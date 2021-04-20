#!/bin/bash


pushd bindings/R/FortMyrmidon

sed -i "s/Version: .*/Version: ${PKG_VERSION}/" DESCRIPTION
${R} CMD INSTALL --preclean --build .

popd
