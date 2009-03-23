#include <string>
#include <vector>

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
};

#define XS_STATE(type, x) \
    INT2PTR(type, SvROK(x) ? SvIV(SvRV(x)) : SvIV(x))

#define XS_STRUCT2OBJ(sv, class, obj) \
    if (obj == NULL) { \
        sv_setsv(sv, &PL_sv_undef); \
    } else { \
        sv_setref_pv(sv, class, (void *) obj); \
    }

namespace pl {
    class Value { };

    class Scalar : public Value {
    public:
        Scalar(SV* _v) {
            this->val = _v;
        }
        Scalar * mortal() {
            sv_2mortal(this->val);
            return this;
        }
        SV * serialize() {
            return val;
        }
    protected:
        SV* val;
    };

    class Int : public Scalar {
    public:
        Int(int _i) : Scalar(newSViv(_i)) {
        }
    };
    class Double : public Scalar {
    public:
        Double(double _i) : Scalar(newSVnv(_i)) {
        }
    };
    class Str : public Scalar {
    public:
        Str(std::string & _s) : Scalar(newSVpv(_s.c_str(), _s.length())) {
        }
    };

    class Reference : public Scalar {
    public:
        Reference(SV*v) : Scalar(v) { }
    };

    class Hash : public Value {
    public:
        Hash(HV* _h) {
            this->h = _h;
        }
        Reference * fetch(const char *key);
    protected:
        HV* h;
    };


    class Ctx {
    public:
        Ctx();
        ~Ctx();
        I32 arg_len() {
            return (I32)(PL_stack_sp - mark);
        }
        int arg_int(int n) {
            return SvIV(fetch_stack(n));
        }
        double arg_double(int n) {
            return SvNV(fetch_stack(n));
        }
        const char* arg_str(int n) {
            return SvPV_nolen(fetch_stack(n));
        }
        Hash * arg_hashref(int n) {
            SV* v = fetch_stack(n);
            if (SvROK(v) && SvTYPE(SvRV(v))==SVt_PVHV) {
                HV* h = (HV*)SvRV(v);
                Hash * hobj = new Hash(h);
                this->register_allocated(hobj);
                return hobj;
            } else {
                Perl_croak(aTHX_ "%s: %s is not a hash reference",
                    "Devel::BindPP",
                    "hv");
            }
        }
        void ret(int n, Scalar* s) {
            PL_stack_base[ax + n] = s ? s->serialize() : &PL_sv_undef;
        }
        void register_allocated(Value* v) {
            allocated.push_back(v);
        }
    protected:
        SV* fetch_stack(int n) {
            return PL_stack_base[this->ax + n];
        }
        I32 ax;
        SV ** mark;
        std::vector<Value*> allocated;
    };
    std::vector<Ctx*> ctxstack;
    Ctx::Ctx() {
        // same as dAXMARK
        this->ax = *PL_markstack_ptr + 1;
        --PL_markstack_ptr;
        this->mark = PL_stack_base + ax - 1;

        ctxstack.push_back(this);
    }
    Ctx::~Ctx() {
        std::vector<Value*>::iterator iter;
        for (iter = allocated.begin(); iter != allocated.end(); iter++) {
            delete *iter;
        }

        PL_stack_sp = PL_stack_base + ax;

        ctxstack.pop_back();
    }

    class CurCtx {
    public:
        static Ctx * get() {
            return ctxstack[ctxstack.size()-1];
        }
    };

    Reference * Hash::fetch(const char* key) {
        // SV**    hv_fetch(HV* tb, const char* key, I32 klen, I32 lval)
        SV ** v = hv_fetch(h, key, strlen(key), 0);
        if (v) {
            Reference * ref = new Reference(*v);
            CurCtx::get()->register_allocated(ref);
            return ref;
        } else {
            return NULL;
        }
    }

    class Package {
    public:
        Package(std::string _pkg) {
            pkg = _pkg;
        }
        void add_method(const char*name, XSUBADDR_t func, const char *file) {
            newXS((pkg + "::" + name).c_str(), func, file);
        }
    private:
        std::string pkg;
    };

    class BootstrapCtx : public Ctx {
    public:
        BootstrapCtx() : Ctx() {
            xs_version_bootcheck();
        }
        ~BootstrapCtx() {
            PL_stack_base[this->ax] = &PL_sv_yes;
            PL_stack_sp = PL_stack_base + this->ax;
        }
    protected:
        // XS_VERSION_BOOTCHECK
        void xs_version_bootcheck() {
            SV *_sv;
            const char *vn = NULL, *module = SvPV_nolen_const(ST(0));
            if (this->arg_len() >= 2) {
                /* version supplied as bootstrap arg */
                _sv = PL_stack_base[ax+1];
            } else {
                /* XXX GV_ADDWARN */
                _sv = get_sv(Perl_form(aTHX_ "%s::%s", module,
                        vn = "XS_VERSION"), FALSE);
                if (!_sv || !SvOK(_sv))
                _sv = get_sv(Perl_form(aTHX_ "%s::%s", module,
                            vn = "VERSION"), FALSE);
            }
            if (_sv) {
                SV *xssv = Perl_newSVpv(aTHX_ XS_VERSION, 0);
                xssv = new_version(xssv);
                if ( !sv_derived_from(_sv, "version") ) {
                    _sv = new_version(_sv);
                }
                if ( vcmp(_sv,xssv) ) {
                    Perl_croak(aTHX_ "%s object version %"SVf" does not match %s%s%s%s %"SVf,
                        module, SVfARG(vstringify(xssv)),
                        vn ? "$" : "", vn ? module : "", vn ? "::" : "",
                        vn ? vn : "bootstrap parameter", SVfARG(vstringify(_sv)));
                }
            }
        }
    };
};

XS(XS_Devel__BindPP_twice) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice");
    }

    int n = c.arg_int(0);

    c.ret(0, pl::Int(n*2).mortal());
}

XS(XS_Devel__BindPP_twice_n) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice_n");
    }

    double n = c.arg_double(0);

    c.ret(0, pl::Double(n*2).mortal());
}

XS(XS_Devel__BindPP_catfoo) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(str)", "Devel::BindPP::catfoo");
    }

    const char* n = c.arg_str(0);
    std::string buf(n);
    buf += "foo";

    c.ret(0, pl::Str(buf).mortal());
}

XS(XS_hv_fetch) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(hv, str)", "Devel::BindPP::hv_fetch");
    }

    pl::Hash* hash = c.arg_hashref(0);
    const char* key = c.arg_str(1);

    pl::Reference * ret = hash->fetch(key);

    c.ret(0, ret);
}

extern "C" {
    XS(boot_Devel__BindPP) {
        pl::BootstrapCtx bc;

        pl::Package pkg("Devel::BindPP");
        pkg.add_method("twice", XS_Devel__BindPP_twice, __FILE__);
        pkg.add_method("catfoo", XS_Devel__BindPP_catfoo, __FILE__);
        pkg.add_method("twice_n", XS_Devel__BindPP_twice_n, __FILE__);
        pkg.add_method("hvref_fetch", XS_hv_fetch, __FILE__);
    }
}

