#include <string>
#include <vector>

// TODO: use Newx instead of new
// TODO: use Safefree instaed of delete
// TODO: return multiple value
// TODO: support scalarref

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
#include <cstdarg>
};

namespace pl {
    class Value {
    public:
        Value(SV* _v) {
            this->val = _v;
        }
        void dump() {
            sv_dump(val);
        }
        void refcnt_inc() {
            SvREFCNT_inc(this->val);
        }
        void refcnt_dec() {
            SvREFCNT_dec(this->val);
        }
        SV* val;
    };

    class Str;
    class UInt;
    class Int;
    class Double;
    class Pointer;

    class Scalar : public Value {
    public:
        Scalar(SV* _v) : Value(_v) { }
        Scalar * mortal() {
            sv_2mortal(this->val);
            return this;
        }
        SV * serialize() {
            return val;
        }
        Str* as_str();
        Int* as_int();
        UInt* as_uint();
        Double* as_double();
        Pointer* as_pointer();
    };

    class Boolean : public Scalar {
    public:
        Boolean(bool b) : Scalar(b ? &PL_sv_yes : &PL_sv_no) { }
        static Boolean* yes();
    };
    class Int : public Scalar {
    public:
        Int(SV* _s) : Scalar(_s) { }
        Int(int _i) : Scalar(newSViv(_i)) { }
        int to_c() {
            return SvIV(this->val);
        }
    };
    class UInt : public Scalar {
    public:
        UInt(SV* _s) : Scalar(_s) { }
        UInt(unsigned int _i) : Scalar(newSVuv(_i)) { }
        unsigned int to_c() {
            return SvUV(this->val);
        }
    };
    class Double : public Scalar {
    public:
        Double(SV* _s) : Scalar(_s) { }
        Double(double _i) : Scalar(newSVnv(_i)) { }
        double to_c() {
            return SvNV(this->val);
        }
    };
    class Str : public Scalar {
    public:
        Str(SV* _s) : Scalar(_s) { }
        Str(std::string & _s) : Scalar(newSVpv(_s.c_str(), _s.length())) { }
        Str(const char* _s) : Scalar(newSVpv(_s, strlen(_s))) { }
        const char* to_c() {
            return SvPV_nolen(this->val);
        }
    };

    class Hash;
    class Array;

    class Reference : public Scalar {
    public:
        Reference(SV*v) : Scalar(v) { }
        static Reference * new_inc(Scalar* thing);
        void bless(const char *pkg) {
            HV * stash = gv_stashpv(pkg, TRUE);
            sv_bless(this->val, stash);
        }
        Hash * as_hash();
        Array * as_array();
    };

    class Hash : public Value {
    public:
        Hash() : Value((SV*)newHV()) { }
        Hash(HV* _h) : Value((SV*)_h) { }
        Reference * fetch(const char *key);
        bool exists(const char*key) {
            return this->exists(key, strlen(key));
        }
        bool exists(const char*key, I32 klen) {
            return hv_exists((HV*)this->val, key, klen);
        }
        Reference* del(const char*key) {
            return this->del(key, strlen(key));
        }
        Reference* del(const char*key, I32 klen);

        Reference* store(const char*key, Scalar*value) {
            return this->store(key, strlen(key), value);
        }
        Reference* store(const char*key, I32 klen, Scalar*value);
        Scalar* scalar();
        void undef();
        void clear();
    };

    class Array : public Value {
    public:
        Array() : Value((SV*)newAV()) { }
        Array(AV* _a) : Value((SV*)_a) { }
        void push(Value * v) {
            av_push((AV*)this->val, v->val);
        }
        void unshift(Int &i) {
            this->unshift(i.to_c());
        }
        void unshift(I32 i) {
            av_unshift((AV*)this->val, i);
        }
        Scalar * pop();
        Scalar * shift();
        Reference * fetch(I32 key);
        I32 len() {
            return av_len((AV*)this->val);
        }
        // TODO: store
        // TODO: shift
        // TODO: unshift
        // TODO: clear
        // TODO: undef
        // TODO: extend
    };

    class Ctx {
    public:
        Ctx();
        ~Ctx();
        I32 arg_len() {
            return (I32)(PL_stack_sp - mark);
        }
        Scalar* arg_scalar(int n) {
            Scalar*s = new Scalar(fetch_stack(n));
            this->register_allocated(s);
            return s;
        }
        Reference * arg_ref(int n) {
            SV* v = fetch_stack(n);
            if (SvROK(v)) {
                Reference * obj = new Reference(v);
                this->register_allocated(obj);
                return obj;
            } else {
                Perl_croak(aTHX_ "%s: %s is not a reference",
                    "Devel::BindPP",
                    "av");
            }
        }
        void ret(int n, Scalar* s) {
            PL_stack_base[ax + n] = s ? s->serialize() : &PL_sv_undef;
        }
        void return_true() {
            this->ret(0, pl::Boolean::yes());
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

    Reference * Reference::new_inc(Scalar* thing) {
        Reference* ref = new Reference(newRV_inc(thing->val));
        CurCtx::get()->register_allocated(ref);
        return ref;
    }

    Reference * Hash::fetch(const char* key) {
        // SV**    hv_fetch(HV* tb, const char* key, I32 klen, I32 lval)
        SV ** v = hv_fetch((HV*)this->val, key, strlen(key), 0);
        if (v) {
            Reference * ref = new Reference(*v);
            CurCtx::get()->register_allocated(ref);
            return ref;
        } else {
            return NULL;
        }
    }
    Reference * Array::fetch(I32 key) {
        SV ** v = av_fetch((AV*)this->val, key, 0);
        if (v) {
            Reference * ref = new Reference(*v);
            CurCtx::get()->register_allocated(ref);
            return ref;
        } else {
            return NULL;
        }
    }
    Scalar * Array::pop() {
        SV* v = av_pop((AV*)this->val);
        Scalar *s = new Scalar(v);
        CurCtx::get()->register_allocated(s);
        return s;
    }
    Scalar * Array::shift() {
        SV* v = av_shift((AV*)this->val);
        Scalar *s = new Scalar(v);
        CurCtx::get()->register_allocated(s);
        return s;
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

    class Pointer : public Scalar {
    public:
        Pointer(SV* s) : Scalar(s) { }
        Pointer(void* _ptr, const char* klass) : Scalar(sv_newmortal()) {
            if (_ptr == NULL) {
                sv_setsv(this->val, &PL_sv_undef);
            } else {
                sv_setref_pv(this->val, klass, _ptr);
            }
        }

        template <class T>
        T* extract() {
            return INT2PTR(T *, SvROK(this->val) ? SvIV(SvRV(this->val)) : SvIV(this->val));
        }
    };

    class Perl {
    public:
//      template<class U>
//      static void* alloc(int size) {
//          void *buf;
//          Newx(buf, size, U);
//          return buf;
//      }
    };

    Str* Scalar::as_str() {
        if (SvPOK(this->val)) {
            Str * s = new Str(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a string",
                "Devel::BindPP",
                "sv");
        }
    }
    Pointer* Scalar::as_pointer() {
        if (SvROK(this->val)) {
            Pointer * s = new Pointer(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a pointer",
                "Devel::BindPP",
                "sv");
        }
    }
    Int* Scalar::as_int() {
        if (SvIOK(this->val)) {
            Int * s = new Int(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a int",
                "Devel::BindPP",
                "sv");
        }
    }
    UInt* Scalar::as_uint() {
        if (SvIOK(this->val)) {
            UInt * s = new UInt(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a uint",
                "Devel::BindPP",
                "sv");
        }
    }
    Double* Scalar::as_double() {
        if (SvNOK(this->val)) {
            Double * s = new Double(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a double",
                "Devel::BindPP",
                "sv");
        }
    }

    Hash * Reference::as_hash() {
        if (SvROK(this->val) && SvTYPE(SvRV(this->val))==SVt_PVHV) {
            HV* h = (HV*)SvRV(this->val);
            Hash * hobj = new Hash(h);
            CurCtx::get()->register_allocated(hobj);
            return hobj;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a hash reference",
                "Devel::BindPP",
                "hv");
        }
    }
    Array * Reference::as_array() {
        SV* v = this->val;
        if (SvROK(v) && SvTYPE(SvRV(v))==SVt_PVAV) {
            AV* a = (AV*)SvRV(v);
            Array * obj = new Array(a);
            CurCtx::get()->register_allocated(obj);
            return obj;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a array reference",
                "Devel::BindPP",
                "av");
        }
    }
    Reference* Hash::del(const char*key, I32 klen) {
        Reference * ref = new Reference(hv_delete((HV*)this->val, key, klen, 0));
        CurCtx::get()->register_allocated(ref);
        return ref;
    }
    Reference* Hash::store(const char*key, I32 klen, Scalar*value) {
        SV ** s = hv_store((HV*)this->val, key, klen, value->val, 0);
        assert(s && SvROK(*s));
        Reference * ref = new Reference(*s);
        CurCtx::get()->register_allocated(ref);
        return ref;
    }
    Scalar* Hash::scalar() {
        Scalar*s = new Scalar(hv_scalar((HV*)this->val));
        CurCtx::get()->register_allocated(s);
        return s;
    }
    void Hash::undef() {
        hv_undef((HV*)this->val);
    }
    void Hash::clear() {
        hv_clear((HV*)this->val);
    }
    Boolean* Boolean::yes() {
        Boolean* s = new Boolean(true);
        CurCtx::get()->register_allocated(s);
        return s;
    }
};
