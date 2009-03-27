use strict;
use warnings;
use Test::More tests => 26;

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

# pointer
{
    my $a = Devel::BindPP::Pointer->new();
    isa_ok $a, 'Devel::BindPP::Pointer';
    is $a->get(), 'ok';
}

# file test
{
    use File::Temp;
    my $tmp = File::Temp->new(UNLINK => 0);
    ok !-d $tmp, 'file test';
    ok !Devel::BindPP::FileTest::is_dir($tmp->filename);
    ok -f $tmp;
    ok ok Devel::BindPP::FileTest::is_file($tmp->filename);
}

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

