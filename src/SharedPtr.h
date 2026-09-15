#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER
#include <cassert>

class ControlBlockBase {
public:
    // default constructor
    ControlBlockBase(): m_refCount{0} {}

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    // implement the destructor.
    virtual ~ControlBlockBase() {
        assert(m_refCount == 0);
    } 

    // pure virtual function; must be overriden by derived classes
    virtual void* managedAddress() = 0;

    // Delete copies, which also implicitly deletes moves.
    ControlBlockBase(const ControlBlockBase&) = delete;
    ControlBlockBase& operator=(const ControlBlockBase&) = delete;

    long increment()
    {
        // increment refcount by 1 and return result.
        return ++m_refCount;
    }

    long decrement()
    {
        // decrement refcount by 1 and return result.
        return --m_refCount;
    }

    long refCount() const
    {
        // just return the refcount.
        return m_refCount;
    }

private:
    // add field(s) which both control block types need to have
    long m_refCount;
};

// 1. Version storing the pointer to the object
template <typename T>
class ControlBlock : public ControlBlockBase {
public:
    ControlBlock(T *ptr): m_storedPtr{ptr} {}

    void *managedAddress() {
        // NOTE: not sure if the explicit cast is needed
        return static_cast<void *>(m_storedPtr);
    }
private:
    T *m_storedPtr;
};

template <typename T>
class SharedPtr {
public:
    // constructors
    // Case 1: no arguments
    SharedPtr(): m_controlBlock{nullptr}, m_storedPtr{nullptr} {};

    // Case 2: pointer given
    SharedPtr(T *ptr): m_controlBlock{new ControlBlock<T>(ptr)}, m_storedPtr{ptr} {
        m_controlBlock->increment();
    }

    // destructor
    ~SharedPtr() {
        m_controlBlock->decrement();
    }

    // copy semantics
    // copy constructor
    SharedPtr(const SharedPtr<T> &sharedPtr): m_storedPtr{sharedPtr.m_storedPtr}, m_controlBlock{sharedPtr.m_controlBlock} {
        m_controlBlock->increment();
    }

    // copy assignment
    SharedPtr<T> &operator=(const SharedPtr<T> &sharedPtr) {
        // NOTE: I assume we need to decrement current control block's count, because it's 1 less thing pointing to it
        if (m_controlBlock) {
            m_controlBlock->decrement();
        }
        m_controlBlock = sharedPtr.m_controlBlock;
        m_storedPtr = sharedPtr.m_storedPtr;
    }

    // move semantics
    // move constructor
    SharedPtr(SharedPtr<T> &&sharedPtr): m_storedPtr{sharedPtr.m_storedPtr}, m_controlBlock{sharedPtr.m_controlBlock} {}

    // move assignment

private:
    // our two raw pointers: 1. control block, 2. stored pointer
    ControlBlock<T> *m_controlBlock;
    T *m_storedPtr;
};


#endif
