use strict;
use Test::More tests => 2;

use Devel::BindPP;

is Devel::BindPP::twice(5), 10;
is Devel::BindPP::catfoo('hoge'), 'hogefoo';

