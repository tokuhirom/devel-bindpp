#include "bindpp.h"
// This is an example file for BindPP

XS(xs_p_new) {
    pl::Ctx c;

    char *self;
    Newx(self, 3, char);
    strcpy(self, "ok");

    pl::Pointer obj((void*)self, "Devel::BindPP::Pointer");
    c.ret(&obj);
}

XS(xs_s_cats) {
    pl::Ctx c;

    pl::Str * s1 = c.arg(0)->as_str();
    pl::Str * s2 = c.arg(1)->as_str();
    pl::Str * s3 = c.arg(2)->as_str();
    pl::Str * s4 = c.arg(3)->as_str();

    s1->concat(s2);
    s1->concat(s3->to_c());
    s1->concat(s4->to_c(), 1);

    c.ret(s1);
}

XS(xs_s_is_object) {
    pl::Ctx c;

    pl::Reference * r = c.arg(0)->as_ref();
    pl::Boolean b( r->is_object() );
    c.ret(&b);
}
XS(xs_s_call_cv) {
    pl::Ctx c;

    pl::Code * code = c.arg(0)->as_ref()->as_code();

    pl::Int i(4649);
    i.mortal();
    pl::Array args;
    pl::Array retval;
    args.push(&i);
    pl::Perl::call(code, G_ARRAY, &args, &retval);
    pl::Reference * ref = pl::Reference::new_inc(&retval);
    c.ret(ref);
}

XS(xs_p_get) {
    pl::Ctx c;

    pl::Pointer * p = c.arg(0)->as_pointer();

    char *self = p->extract<char>();
    pl::Str s(self);
    c.ret(&s);
}

XS(xs_p_destroy) {
    pl::Ctx c;

    pl::Pointer * p = c.arg(0)->as_pointer();
    Safefree(p->extract<char>());
    c.ret(pl::Boolean::yes());
}

XS(XS_Devel__BindPP_twice) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice");
    }

    int n = c.arg(0)->as_int()->to_c();

    c.ret(pl::Int(n*2).mortal());
}

XS(xs_twice_deref) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice");
    }

    int n = c.arg(0)->as_ref()->as_scalar()->as_int()->to_c();

    c.ret(pl::Int(n*2).mortal());
}

XS(XS_Devel__BindPP_twice_n) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice_n");
    }

    double n = c.arg(0)->as_double()->to_c();

    c.ret(pl::Double(n*2).mortal());
}

XS(XS_Devel__BindPP_twice_u) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice_u");
    }

    unsigned int n = c.arg(0)->as_uint()->to_c();

    c.ret(pl::UInt(n*2).mortal());
}

XS(XS_Devel__BindPP_catfoo) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(str)", "Devel::BindPP::catfoo");
    }

    const char* n = c.arg(0)->as_str()->to_c();
    std::string buf(n);
    buf += "foo";

    c.ret(pl::Str(buf).mortal());
}

XS(XS_hv_fetch) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(hv, str)", "Devel::BindPP::hv_fetch");
    }

    pl::Hash* hash = c.arg(0)->as_ref()->as_hash();
    const char* key = c.arg(1)->as_str()->to_c();

    pl::Reference * ret = hash->fetch(key);

    c.ret(ret);
}

XS(xs_av_fetch) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    const int key = c.arg(1)->as_int()->to_c();

    pl::Reference * ret = array->fetch(key);

    c.ret(ret);
}

XS(xs_av_push) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Scalar*s = c.arg(1);
    array->push(s);
}

XS(xs_av_unshift) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Int*i = c.arg(1)->as_int();
    array->unshift(i->to_c());
    c.return_true();
}

XS(xs_av_store) {
    pl::Ctx c;

    if (c.arg_len() != 3) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Int*i = c.arg(1)->as_int();
    pl::Scalar *val = c.arg(2);
    array->store(i->to_c(), val);
    c.return_true();
}

XS(xs_av_clear) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    array->clear();

    c.return_true();
}

XS(xs_av_undef) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    array->undef();

    c.return_true();
}

XS(xs_av_pop) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Scalar* s = array->pop();
    c.ret(s);
}

XS(xs_av_shift) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Scalar * s =  array->shift();
    c.ret(s);
}

XS(xs_av_len) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Int i(array->len());
    c.ret(&i);
}

XS(do_bless) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Reference* ref = c.arg(0)->as_ref();
    const char * pkg = c.arg(1)->as_str()->to_c();
    ref->bless(pkg);

    c.ret(ref);
}

XS(XS_hv_exists) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Hash* hash = c.arg(0)->as_ref()->as_hash();
    const char * key = c.arg(1)->as_str()->to_c();

    pl::Boolean b(hash->exists(key));

    c.ret(&b);
}

XS(XS_hv_delete) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Hash* hash = c.arg(0)->as_ref()->as_hash();
    const char * key = c.arg(1)->as_str()->to_c();

    pl::Reference * ref = hash->del(key);

    c.ret(ref);
}

XS(xs_refcnt_inc) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Scalar * s = c.arg(0);
    s->refcnt_inc();
    c.ret(s);
}

XS(xs_refcnt_dec) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Scalar * s = c.arg(0);
    s->refcnt_dec();
    c.ret(s);
}

XS(xs_to_c) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Str * s = c.arg(0)->as_str();
    Perl_croak(aTHX_ "OK: '%s'", s->to_c());
}

XS(xs_hv_store) {
    pl::Ctx c;

    if (c.arg_len() != 3) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg(0)->as_ref()->as_hash();
    const char * key = c.arg(1)->as_str()->to_c();
    pl::Scalar * val = c.arg(2);

    pl::Reference *ref = h->store(key, val);

    c.ret(ref);
}

XS(xs_hv_scalar) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg(0)->as_ref()->as_hash();
    c.ret(h->scalar());
}

XS(xs_hv_undef) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg(0)->as_ref()->as_hash();
    h->undef();
    c.return_true();
}
XS(xs_hv_clear) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg(0)->as_ref()->as_hash();
    h->clear();
    c.ret(pl::Boolean::yes());
}

XS(xs_basic_mult) {
    pl::Ctx c;

    if (c.arg_len() != 0) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Int s1(4);
    pl::Int s2(9);
    std::vector<pl::Scalar*> v;
    v.push_back(&s1);
    v.push_back(&s2);

    c.return_multi(v);
}
XS(xs_basic_mult2) {
    pl::Ctx c;

    if (c.arg_len() != 0) {
       Perl_croak(aTHX_ "orz");
    }

    std::vector<pl::Scalar*> v;

    c.return_multi(v);
}
XS(xs_wantarray) {
    pl::Ctx c;

    if (c.arg_len() != 0) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Str y("yes");
    pl::Str n("no");
    if (c.wantarray()) {
        c.ret(&y);
    } else {
        c.ret(&n);
    }
}

/*
XS(xs_pkg_stash) {
    pl::Ctx c;

    pl::Str *pkg = c.arg(0)->as_str();
    pl::Hash * h = pl::Perl::get_stash(pkg);
    pl::Reference * ref = pl::Reference::new_inc(h);
    c.ret(ref);
}
*/

extern "C" {
    XS(boot_Devel__BindPP) {
        pl::BootstrapCtx bc;

        pl::Package b("Devel::BindPP::Basic");
        b.add_method("mult", xs_basic_mult, __FILE__);
        b.add_method("mult2", xs_basic_mult2, __FILE__);
        b.add_method("wantarray", xs_wantarray, __FILE__);

        pl::Package s("Devel::BindPP::Scalar");
        s.add_method("twice", XS_Devel__BindPP_twice, __FILE__);
        s.add_method("catfoo", XS_Devel__BindPP_catfoo, __FILE__);
        s.add_method("twice_n", XS_Devel__BindPP_twice_n, __FILE__);
        s.add_method("twice_u", XS_Devel__BindPP_twice_u, __FILE__);
        s.add_method("do_bless", do_bless, __FILE__);
        s.add_method("refcnt_inc", xs_refcnt_inc, __FILE__);
        s.add_method("refcnt_dec", xs_refcnt_dec, __FILE__);
        s.add_method("to_c", xs_to_c, __FILE__);
        s.add_method("twice_deref", xs_twice_deref, __FILE__);
        s.add_method("cats", xs_s_cats, __FILE__);
        s.add_method("is_object", xs_s_is_object, __FILE__);
        s.add_method("call_cv", xs_s_call_cv, __FILE__);

        // Hash
        pl::Package h("Devel::BindPP::Hash");
        h.add_method("hvref_fetch", XS_hv_fetch, __FILE__);
        h.add_method("exists", XS_hv_exists, __FILE__);
        h.add_method("delete", XS_hv_delete, __FILE__);
        h.add_method("store", xs_hv_store, __FILE__);
        h.add_method("scalar", xs_hv_scalar, __FILE__);
        h.add_method("undef", xs_hv_undef, __FILE__);
        h.add_method("clear", xs_hv_clear, __FILE__);

        // Array
        pl::Package a("Devel::BindPP::Array");
        a.add_method("avref_fetch", xs_av_fetch, __FILE__);
        a.add_method("push", xs_av_push, __FILE__);
        a.add_method("pop", xs_av_pop, __FILE__);
        a.add_method("len", xs_av_len, __FILE__);
        a.add_method("shift", xs_av_shift, __FILE__);
        a.add_method("unshift", xs_av_unshift, __FILE__);
        a.add_method("store", xs_av_store, __FILE__);
        a.add_method("clear", xs_av_clear, __FILE__);
        a.add_method("undef", xs_av_undef, __FILE__);

        // Pointer
        pl::Package p("Devel::BindPP::Pointer");
        p.add_method("new", xs_p_new, __FILE__);
        p.add_method("get", xs_p_get, __FILE__);
        p.add_method("DESTROY", xs_p_destroy, __FILE__);

        // package
        /*
        {
            pl::Package pkg("Devel::BindPP::Package");
            pkg.add_method("stash", xs_pkg_stash, __FILE__);
        }
        */
    }
}

