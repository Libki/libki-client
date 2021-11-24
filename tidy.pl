#!/usr/bin/perl

opendir my $dir, "." or die "Cannot open directory: $!";
my @files = readdir $dir;
closedir $dir;

foreach my $file ( @files ) {
    next unless $file =~ /\.cpp$/ || $file =~ /\.h$/;
    print "Working on $file\n";
    qx{clang-format --style=file $file > $file.new};
    qx{mv $file.new $file};
}
