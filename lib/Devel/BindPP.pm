package Devel::BindPP;
use strict;
use warnings;
our $VERSION = '0.02';
our @ISA;

eval {
    require XSLoader;
    XSLoader::load(__PACKAGE__, $VERSION);
    1;
} or do {
    require DynaLoader;
    push @ISA, 'DynaLoader';
    __PACKAGE__->bootstrap($VERSION);
};

1;
__END__

=head1 NAME

Devel::BindPP - bind c++ to perl

=head1 SYNOPSIS

    #include "bindpp.h"

    XS(xs_new) {
        pl::Ctx c;

        char *self;
        Newx(self, 3, char);
        strcpy(self, "ok");

        pl::Pointer obj((void*)self, "MyOwnExt");
        c.ret(&obj);
    }

    XS(xs_get) {
        pl::Ctx c;

        pl::Pointer * p = c.arg(0)->as_pointer();

        char *self = p->extract<char>();
        pl::Str s(self);
        c.ret(&s);
    }

    XS(xs_destroy) {
        pl::Ctx c;

        pl::Pointer * p = c.arg(0)->as_pointer();
        Safefree(p->extract<char>());
        c.ret(pl::Boolean::yes());
    }

    extern "C" {
        XS(bootstrap_Your__Package) {
            pl::BootstrapCtx bc;

            pl::Package pkg("MyOwnExt");
            pkg.add_method("new", xs_new, __FILE__);
            pkg.add_method("get", xs_get, __FILE__);
            pkg.add_method("DESTROY", xs_destroy, __FILE__);
        }
    };

=head1 CAUTION

THIS MODULE IS ITS IN BETA QUALITY. API MAY CHANGE IN THE FUTURE.

=head1 DESCRIPTION

XS is too difficult for me :p XS is filled by too much C macros.

You can write perl extension by pure C++ with Devel::BindPP.

=head1 AUTHOR

Tokuhiro Matsuno E<lt>tokuhirom ah! gmail.comE<gt>

=head1 SEE ALSO

=head1 LICENSE

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself.

=cut
