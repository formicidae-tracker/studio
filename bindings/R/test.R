devtools::load_all("FortMyrmidon");
e <- fmExperimentOpenReadOnly("/home/atuleu/Documents/large.myrmidon");
start <- fmTimeParse('2020-05-14T08:00:00.000Z');
end <- fmTimeParse('2020-05-17T08:01:00.000Z');
gap <- fmSecond(3);
dd <- fmQueryComputeAntInteractions(e,
                                    start = start,
                                    end = end,
                                    maximumGap = fmSecond(20),
                                    singleThreaded = FALSE,
                                    showProgress = TRUE,
                                    reportTrajectories = TRUE);
#dd$interactions
#dd$summaryTrajectory
