devtools::load_all("fm");
e <- fmExperimentOpenReadOnly("/home/atuleu/Documents/large.myrmidon");
start <- fmTimeParse('2020-05-14T08:00:00.000Z')$const_ptr();
end <- fmTimeParse('2020-05-14T10:00:00.000Z')$const_ptr();
dd <- fmQueryIdentifyFrames(e,start,end,FALSE,TRUE,TRUE);
lastDD <- dd[[length(dd)]]
lastDD@frameTime
lastDD@data
