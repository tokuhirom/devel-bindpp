use strict;
use warnings;
use Test::More tests => 5;

use Devel::BindPP;

is Devel::BindPP::twice(5), 10;
is Devel::BindPP::catfoo('hoge'), 'hogefoo';
is Devel::BindPP::twice_n(3.2), 6.4;
is Devel::BindPP::hvref_fetch(+{a => 'b'}, 'a'), 'b';
is Devel::BindPP::hvref_fetch(+{a => 'b'}, 'c'), undef;

