
#include <napi.h>
#include "boost-share-cache.cc"

class NodeShareCache : public Napi::ObjectWrap<NodeShareCache>
{

private:
    BoostShareCache *bsc;

public:
    NodeShareCache(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<NodeShareCache>(info)
    {
        Napi::Env env = info.Env();

        int length = info.Length();
        if (length <= 0 || !info[0].IsString())
        {
            Napi::TypeError::New(env, " arg0 string expected").ThrowAsJavaScriptException();
            return;
        }
        std::string name = info[0].As<Napi::String>().Utf8Value();

        long size = 1024 * 4 * 10;
        if (length > 1 && info[1].IsNumber())
        {
            Napi::Number sizeNum = info[1].As<Napi::Number>();
            size = sizeNum.Int32Value();
        }
        bool renew = false;
        if (length > 2 && info[2].IsBoolean())
        {
            Napi::Boolean renewBool = info[2].As<Napi::Boolean>();
            renew = renewBool.Value();
        }
        // cache
        bsc = new BoostShareCache(name, size, renew);
    }

private:
    Napi::Value res(const Napi::CallbackInfo &info, stdstring &v)
    {
        Napi::Env env = info.Env();
        return Napi::String::New(env, v.c_str());
    }
    Napi::Value get(const Napi::CallbackInfo &info)
    {
        int length = info.Length();
        if (length <= 0 || !info[0].IsString())
        {
            Napi::TypeError::New(info.Env(), " arg0 string expected").ThrowAsJavaScriptException();
            return Napi::String::New(info.Env(), "null");
        }
        std::string arg0 = info[0].As<Napi::String>().Utf8Value();
        std::optional<std::string> res = bsc->get(arg0);
        if(res.has_value() == 1){
            return this->res(info, *res);
        }else{
            return info.Env().Undefined();
        }
    }
    Napi::Value stat(const Napi::CallbackInfo &info)
    {
        BoostShareCacheStat stat = bsc->stat();
        Napi::Env env = info.Env();
        Napi::Object obj = Napi::Object::New(env);
        obj.Set("max_size", stat.max_size);
        obj.Set("total_size", stat.total_size);
        obj.Set("free_size", stat.free_size);
        obj.Set("used_size", stat.used_size);
        obj.Set("grow_count", stat.grow_count);
        obj.Set("key_cont", stat.key_cont);
        obj.Set("last_clean_time", stat.last_clean_time);
        obj.Set("version", stat.version);
        return obj;
    }
    Napi::Value remove(const Napi::CallbackInfo &info)
    {
        int length = info.Length();
        if (length <= 0 || !info[0].IsString())
        {
            Napi::TypeError::New(info.Env(), " arg0 string expected").ThrowAsJavaScriptException();
            return Napi::String::New(info.Env(), "null");
        }
        std::string arg0 = info[0].As<Napi::String>().Utf8Value();
        bsc->remove(arg0);
        return info.Env().Undefined();
    }
    Napi::Value set(const Napi::CallbackInfo &info)
    {
        int length = info.Length();
        if (length <= 1 || !info[0].IsString() || !info[1].IsString())
        {
            Napi::TypeError::New(info.Env(), " arg0 string expected").ThrowAsJavaScriptException();
            return Napi::Number::New(info.Env(), 0);
        }

        std::string arg0 = info[0].As<Napi::String>().Utf8Value();
        std::string arg1 = info[1].As<Napi::String>().Utf8Value();

        if (length == 3 && info[2].IsNumber())
        {
            Napi::Number sizeNum = info[2].As<Napi::Number>();
            long maxAge = sizeNum.Int32Value();
            bsc->insert(arg0, arg1, maxAge);
            return Napi::Number::New(info.Env(), 1);
        }
        else
        {
            bsc->insert(arg0, arg1);
            return Napi::Number::New(info.Env(), 1);
        }
    }
    Napi::Value setMaxSize(const Napi::CallbackInfo &info)
    {
        int length = info.Length();
        if (length <= 0 || !info[0].IsNumber())
        {
            Napi::TypeError::New(info.Env(), " arg0 number expected").ThrowAsJavaScriptException();
            return Napi::Number::New(info.Env(), 0);
        }

        Napi::Number sizeNum = info[0].As<Napi::Number>();
        long size = sizeNum.Int32Value();
        this->bsc->setMaxSize(size);
        return Napi::Number::New(info.Env(), 1);
    }

    Napi::Value setMaxAge(const Napi::CallbackInfo &info)
    {
        int length = info.Length();
        if (length <= 0 || !info[0].IsNumber())
        {
            Napi::TypeError::New(info.Env(), " arg0 number expected").ThrowAsJavaScriptException();
            return Napi::Number::New(info.Env(), 0);
        }

        Napi::Number sizeNum = info[0].As<Napi::Number>();
        long size = sizeNum.Int32Value();
        this->bsc->setMaxAge(size);
        return Napi::Number::New(info.Env(), 1);
    }
    Napi::Value setLogLevel(const Napi::CallbackInfo &info)
    {
        int length = info.Length();
        if (length <= 0 || !info[0].IsNumber())
        {
            Napi::TypeError::New(info.Env(), " arg0 number expected").ThrowAsJavaScriptException();
            return Napi::Number::New(info.Env(), 0);
        }

        Napi::Number sizeNum = info[0].As<Napi::Number>();
        int lv = sizeNum.Int32Value();
        this->bsc->setLogLevel(static_cast<nnd::ELevel>(lv));
        return Napi::Number::New(info.Env(), 1);
    }

    Napi::Value setLock(const Napi::CallbackInfo &info)
    {
        int length = info.Length();
        if (length <= 0 || !info[0].IsBoolean())
        {
            Napi::TypeError::New(info.Env(), " arg0 bool expected").ThrowAsJavaScriptException();
            return Napi::Number::New(info.Env(), 0);
        }

        Napi::Boolean enable = info[0].As<Napi::Boolean>();
        this->bsc->setLock(enable.Value());
        return Napi::Number::New(info.Env(), 1);
    }

    Napi::Value destroy(const Napi::CallbackInfo &info)
    {
        this->bsc->destroy();
        return Napi::Number::New(info.Env(), 1);
    }

public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports)
    {
        Napi::Function func =
            DefineClass(env,
                        "NodeShareCache",
                        {InstanceMethod("get", &NodeShareCache::get),
                         InstanceMethod("del", &NodeShareCache::remove),
                         InstanceMethod("set", &NodeShareCache::set),
                         InstanceMethod("destroy", &NodeShareCache::destroy),
                         InstanceMethod("setLogLevel", &NodeShareCache::setLogLevel),
                         InstanceMethod("setMaxAge", &NodeShareCache::setMaxAge),
                         InstanceMethod("setLock", &NodeShareCache::setLock),
                         InstanceMethod("stat", &NodeShareCache::stat),
                         InstanceMethod("setMaxSize", &NodeShareCache::setMaxSize)});

        Napi::FunctionReference *constructor = new Napi::FunctionReference();
        *constructor = Napi::Persistent(func);
        env.SetInstanceData(constructor);

        exports.Set("NodeShareCache", func);
        return exports;
    }
};
