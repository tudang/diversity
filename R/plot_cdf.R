#!/usr/local/bin/Rscript

# Load the necessary packages.
library("reshape2")
library("plyr")
library("scales")
library("ggplot2")

args = commandArgs(trailingOnly=TRUE)

my_theme <- function() {
    theme(panel.grid.major.x = element_blank(),
        panel.grid.minor.x = element_blank(),
        text = element_text(size=34, family='Times'),
        axis.title.y=element_text(margin=margin(0,10,0,0)),
        axis.title.x=element_text(margin=margin(10,0,0,0)),
        legend.text = element_text(size=20, family='Times'),
        legend.position = c(.8, .6)
    )
}

plot_cdf <- function(fname) {
    df <- read.csv(fname, col.names=c('latency'), colClasses=c("numeric"))
    df$latency <- df$latency * 10^6
    print(summary(df))
    pdf("cdf_latency.pdf")
    ggplot(df, aes(x=latency)) +
    stat_ecdf() +
    theme_bw() +
    my_theme() +
    xlab('Latency (\U00B5s)') +
    ylab('CDF')
    # scale_x_continuous(limits=c(0, 1000000))
}


input_file <- ifelse(length(args)==0, "csv/latency.csv", args[1])
print(input_file)
plot_cdf(input_file)
