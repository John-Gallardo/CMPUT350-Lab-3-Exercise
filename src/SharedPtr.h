#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER
#include <cassert>
#include <utility>

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
    // constructor
    ControlBlock(T *ptr): m_storedPtr{ptr} {}

    // destructor
    ~ControlBlock() override {
        delete m_storedPtr;
    }

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
        if (m_controlBlock) {
            m_controlBlock->decrement();
            m_controlBlock = nullptr;
        }
    }

    // copy semantics
    // copy constructor
    SharedPtr(const SharedPtr<T> &sharedPtr): m_storedPtr{sharedPtr.m_storedPtr}, m_controlBlock{sharedPtr.m_controlBlock} {
        if (m_controlBlock) {
            m_controlBlock->increment();
        }
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
    SharedPtr(SharedPtr<T> &&sharedPtr): m_storedPtr{sharedPtr.m_storedPtr}, m_controlBlock{sharedPtr.m_controlBlock} {
        sharedPtr.m_storedPtr = nullptr;
        sharedPtr.m_controlBlock = nullptr;
    }

    // move assignment
    SharedPtr<T> &operator=(SharedPtr<T> &&sharedPtr) {
        m_storedPtr = sharedPtr.m_storedPtr;
        if (m_controlBlock) {
            m_controlBlock->decrement();
        }
        m_controlBlock = sharedPtr.m_controlBlock;

        sharedPtr.m_storedPtr = nullptr;
        sharedPtr.m_controlBlock = nullptr;
    }

    // 'dereference' operators
    T& operator*() {
        return *m_storedPtr;
    }

    T* operator->() {
        return m_storedPtr;
    }

    T* get() {
        return m_storedPtr;
    }

    // equality & boolean conversion operators
    bool operator==(const SharedPtr<T> &other) {
        return other.m_storedPtr == m_storedPtr;
    }

    operator bool() const {
        return m_storedPtr != nullptr;
    }

    // shared pointer swap
    void swap(SharedPtr<T> &other) {
        // swap pointers & control block pointers.
        std::swap(other.m_storedPtr, m_storedPtr);
        std::swap(other.m_controlBlock, m_controlBlock);
        // NOTE: I believe nothing should get decremented or incremented because the number of pointers remain the same?
    }

    // reset
    void reset() {
        if (m_controlBlock) {
            m_controlBlock->decrement();
        }
        m_controlBlock = nullptr;
        m_storedPtr = nullptr;
    }

    // reset override
    void reset(T* other) {
        // Edge Case: self-assignment
        if (other == m_storedPtr) {
            return;
        }

        // decrement current control block & create a new shared ptr manually
        if (m_controlBlock) {
            m_controlBlock->decrement();
        }

        m_controlBlock = new ControlBlock(other);
        m_controlBlock->increment();
        m_storedPtr = other;
    }

    long useCount() {
        if (m_controlBlock) {
            return m_controlBlock->refCount();
        }
        return 0;
    }

private:
    // our two raw pointers: 1. control block, 2. stored pointer
    ControlBlockBase *m_controlBlock;
    T *m_storedPtr;
};

template <typename T, typename... Args>
SharedPtr<T> makeSharedBasic(Args&&... args) {
    // NOTE: I assume it's basically identical to prep's makeUnique()?
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif
