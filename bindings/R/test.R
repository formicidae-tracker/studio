devtools::load_all("FortMyrmidon");
e <- fmExperimentOpenReadOnly("/home/atuleu/Documents/large.myrmidon");
start <- fmTimeParse('2020-05-14T08:00:00.000Z');
end <- fmTimeParse('2020-05-15T08:00:00.000Z');
gap <- fmSecond(3);
#dd <- fmQueryIdentifyFrames(e,start,end,TRUE,FALSE,TRUE);
dd <- fmQueryComputeAntInteractions(e,start,end,gap,showProgress = TRUE);
