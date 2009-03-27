use strict;
use warnings;
use Test::More tests => 2;

use Devel::BindPP;

# pointer
{
    my $a = Devel::BindPP::Pointer->new();
    isa_ok $a, 'Devel::BindPP::Pointer';
    is $a->get(), 'ok';
}

