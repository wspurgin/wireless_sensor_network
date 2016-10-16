library(readr)

# Sen input file via commandline
argv <- commandArgs(FALSE)

if (length(argv) < 1) {
  warning("At least one file must be listed when invoking this script")
}

rgg_files <- argv

sapply(rgg_files, function(rgg_file) {
  rgg <- read_csv(rgg_file, col_names = c("x", "y"))
  dev <- plot(rgg)
  image_name <- paste(gsub("\\.csv", "", rgg_file), "png", sep = ".")
  png(image_name, width=4, height=4, units="in", res=300)
  par(mar=c(4,4,1,1))
  plot(rgg, cex=0.5)
  dev.off() #only 129kb in size
})
