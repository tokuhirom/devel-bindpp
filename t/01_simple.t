use strict;
use warnings;
use Test::More tests => 5;

use Devel::BindPP;

# basic
{
    my $a = join ',', Devel::BindPP::Basic::mult();
    is $a, '4,9';
}
{
    is Devel::BindPP::Basic::mult2(), undef;
}
{
    my @a = Devel::BindPP::Basic::wantarray();
    is "@a", 'yes';
    my $a = Devel::BindPP::Basic::wantarray();
    is "$a", 'no';
}
is Devel::BindPP::Basic::FOO, 1981;

# package
=pod
{
    is_deeply(
        Devel::BindPP::Package::stash('Devel::BindPP::Pointer'),
        {
            'isa'     => *Devel::BindPP::Pointer::isa,
            'get'     => *Devel::BindPP::Pointer::get,
            'DESTROY' => *Devel::BindPP::Pointer::DESTROY,
            'new'     => *Devel::BindPP::Pointer::new
        }
    );
}

=cut

