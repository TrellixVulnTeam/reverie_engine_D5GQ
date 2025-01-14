#pragma once

namespace rev {

/// @brief Template for making a shared pointer of a class with only protected constructors
template<typename Obj, typename... Args>
inline std::shared_ptr<Obj> prot_make_shared(Args&&... args)
{
    struct helper : public Obj
    {
        helper(Args&&... args)
            : Obj{ std::forward< Args >(args)... }
        {}
    };

    return std::make_shared<helper>(std::forward< Args >(args)...);
}

/// @brief Template for making a unique pointer of a class with only protected constructors
template<typename Obj, typename... Args>
inline std::unique_ptr<Obj> prot_make_unique(Args&&... args)
{
    struct helper : public Obj
    {
        helper(Args&&... args)
            : Obj{ std::forward< Args >(args)... }
        {}
    };

    return std::make_unique<helper>(std::forward< Args >(args)...);
}


} // End rev namespace
