use strict;
use warnings;
use Test::More tests => 3;

use Devel::BindPP;

is Devel::BindPP::twice(5), 10;
is Devel::BindPP::catfoo('hoge'), 'hogefoo';
is Devel::BindPP::twice_n(3.2), 6.4;

