devtools::load_all("fm");
e <- fmExperimentOpenReadOnly("/home/atuleu/Documents/large.myrmidon");
start <- fmTimeParse('2020-05-14T08:00:00.000Z')$const_ptr();
end <- fmTimeParse('2020-05-15T08:00:00.000Z')$const_ptr();
gap <- fmSecond(3);
#dd <- fmQueryIdentifyFrames(e,start,end,TRUE,FALSE,TRUE);
dd <- fmQueryComputeAntInteractionsC(e,start,end,gap,fmMatcherAny(),FALSE,TRUE,FALSE);
