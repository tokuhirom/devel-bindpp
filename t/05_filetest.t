use strict;
use warnings;
use Test::More tests => 4;
use File::Temp;

use Devel::BindPP;

# file test

{
    my $tmp = File::Temp->new(UNLINK => 0);
    ok !-d $tmp, 'file test';
    ok !Devel::BindPP::FileTest::is_dir($tmp->filename);
    ok -f $tmp;
    ok Devel::BindPP::FileTest::is_file($tmp->filename);
    unlink $tmp;
}

