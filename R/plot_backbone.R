read_graph <- function(shape, nodes, average, dir="..") {
  rgg_graph_file <- paste(shape, nodes, average, "graph.csv", sep="_")
  rgg_graph_file <- paste(dir, rgg_graph_file, sep="/")

  adj_file <- paste(shape, nodes, average, "adj.csv", sep="_")
  adj_file <- paste(dir, adj_file, sep="/")

  rgg <- read.csv(rgg_graph_file)
  rgg <- rgg[order(rgg$id),]
  adj <- read.csv(adj_file)
  return(list(rgg=rgg, adj=adj))
}

plot_graph <- function (rgg, adj) {
  rgg <- rgg[order(rgg$id),]
  lim <- c(0,1)
  plot(rgg[,c("x","y")],
       col = ifelse(!is.na(rgg$degree) & rgg$degree == min(rgg$degree), "red", "black"),
       pch = ifelse(!is.na(rgg$degree) & rgg$degree == min(rgg$degree), 16, 1),
       ylim = lim,
       xlim = lim)
  apply(adj, MARGIN = 1, FUN = function(edge) {
  arrows(x0 = as.double(as.matrix(rgg[which(rgg$id == edge[1]), "x"])),
         y0 = as.double(as.matrix(rgg[which(rgg$id == edge[1]), "y"])),
         x1 = as.double(as.matrix(rgg[which(rgg$id == edge[2]), "x"])),
         y1 = as.double(as.matrix(rgg[which(rgg$id == edge[2]), "y"])),
         length = 0)
  })
}

cut_min <- function(rgg, adj) {
  rgg <- rgg[order(rgg$id),]
  row_cut <- which(rgg$degree == min(rgg$degree))
  if (length(row_cut) > 1) {
    row_cut <- row_cut[1]
  }
  node <- rgg[row_cut,]
  rgg <- rgg[-row_cut,]

  p_cut <- which(adj$p_id == node$id)
  c_cut <- which(adj$c_id == node$id)
  adj <- adj[-c(p_cut, c_cut),]

  return(list(rgg=rgg, adj=adj))
}

res <- read_graph("plane", 20, 10, "walkthru")
rgg <- res$rgg
adj <- res$adj

plot_graph(rgg, adj)
res <- cut_min(rgg, adj)
rgg <- res$rgg
adj <- res$adj

for (i in 1:9) {
    res <- cut_min(rgg, adj)
    rgg <- res$rgg
    adj <- res$adj
}
plot_graph(rgg, adj)
