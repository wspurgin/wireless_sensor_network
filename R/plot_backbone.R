library(readr)
library(dplyr)

argv <- commandArgs(TRUE)
print(argv)

if (length(argv) < 1) {
  warning("At least one file must be listed when invoking this script")
}

rgg_graph_file <- argv[1]
rgg_degree_file <- argv[2]


rgg <- read_csv(rgg_graph_file, col_types = cols(backbone = col_integer()))
color_tally <- rgg %>% group_by(color) %>% tally(sort = TRUE)
# cell_size <- if_else(rgg$color == color_tally$color[1:2], 0.5, 0.25)

image_name <- paste(gsub("\\.csv", "", rgg_graph_file), "png", sep = ".")
png(image_name, width=4, height=4, units="in", res=300)
orig <- par(pty="s", mar=c(4,4,1,1))

dev <- rgg[if_else(rgg$color == color_tally$color[1:2], TRUE, FALSE),] %>%
  select(x, y) %>%
  plot(col = c(1,2), cex=0.5)

dev.off() #only 129kb in size


# TODO plot edges
#

# TODO Plot backbones, other points very small
#
par(orig)

degree_when_deleted <- read_csv(rgg_degree_file)
degree_max = max(degree_when_deleted$original_degree)
y_axis_max = degree_max + degree_max / 10

image_name <- paste(gsub("\\.csv", "", rgg_degree_file), "png", sep = ".")
png(image_name)

dev = plot(degree_when_deleted$iteration, degree_when_deleted$degree_when_deleted,
     col = "blue", type="l", ylim=c(0, y_axis_max), ylab = "Degree",
     xlab = "Iteration")
lines(degree_when_deleted$iteration, degree_when_deleted$original_degree,
      col = "red",
      lty=2)
legend(0, y_axis_max, legend = c("Degree When Deleted", "Original Degree"),
       col = c("blue", "red"), lty=c(1,2))

dev.off() #only 129kb in size
