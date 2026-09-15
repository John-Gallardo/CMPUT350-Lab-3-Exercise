#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

class ControlBlockBase {
public:
    // default constructor
    ControlBlockBase(): m_refCount{0} {}

    // dtor is virtual, so that we can call derived class's dtor from a ptr to this base class.
    // TODO: implement the destructor.
    virtual ~ControlBlockBase() {
        
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
    // 1. reference count
    // NOTE: I am assuming just reference count because there could be 2 versions -> 1 storing ptr, 1 embedding the data directly
    int m_refCount;
};


class ControlBlock : public ControlBlockBase {
public:
    void *managedAddress() {

    }
private:

};

template <typename T>
class SharedPtr {
public:
    // constructors
    // Case 1: no arguments
    SharedPtr(): m_controlBlock{nullptr}, m_storedPtr{nullptr} {};

    // Case 2: pointer given
    SharedPtr(T *ptr): m_controlBlock{new ControlBlock()}, m_storedPtr{ptr} {
        m_controlBlock->increment();
    }

    // destructor
    ~SharedPtr() {
        m_controlBlock->decrement();
    }

    // copy semantics
//    SharedPtr(const SharedPtr<T> &sharedPtr) : m_


private:
    // our two raw pointers: 1. control block, 2. stored pointer
    ControlBlock *m_controlBlock;
    T *m_storedPtr;
};


#endif
