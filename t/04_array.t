use strict;
use warnings;
use Test::More tests => 14;

use Devel::BindPP;

# array
is Devel::BindPP::Array::avref_fetch([qw/a b c/], 1), 'b';
is Devel::BindPP::Array::avref_fetch([qw/a b c/], 9), undef, 'fetch';
{
    my $a = [];
    is Devel::BindPP::Array::len($a), -1;
    Devel::BindPP::Array::push($a, 'a');
    is Devel::BindPP::Array::len($a), 0, 'len';
    is_deeply($a, ['a']);
    is Devel::BindPP::Array::pop($a), 'a';
    is Devel::BindPP::Array::len($a), -1;
    is Devel::BindPP::Array::pop($a), undef;
    is Devel::BindPP::Array::shift($a), undef, 'shift';
    Devel::BindPP::Array::push($a, 'a');
    is Devel::BindPP::Array::shift($a), 'a';
    Devel::BindPP::Array::unshift($a, 4);
    is_deeply $a, [undef, undef, undef, undef];
    Devel::BindPP::Array::store($a, 2, 3);
    is_deeply $a, [undef, undef, 3, undef];
    Devel::BindPP::Array::clear($a);
    is_deeply $a, [];
    $a = [1];
    Devel::BindPP::Array::undef($a);
    is_deeply $a, [];
}

