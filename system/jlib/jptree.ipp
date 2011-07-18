/*##############################################################################

    Copyright (C) 2011 HPCC Systems.

    All rights reserved. This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
############################################################################## */


#ifndef _PTREE_IPP
#define _PTREE_IPP

#include "jarray.hpp"
#include "jexcept.hpp"
#include "jhash.hpp"
#include "jmutex.hpp"
#include "jsuperhash.hpp"


#include "jptree.hpp"
#include "jbuff.hpp"

#ifdef __64BIT__
#pragma pack(push,1)    // 64bit pack PTree's    (could possibly do for 32bit also but may be compatibility problems)
#endif



#define ANE_APPEND -1
#define ANE_SET -2
///////////////////
class MappingStringToOwned : public MappingStringTo<IInterfacePtr,IInterfacePtr>
{
public:
  MappingStringToOwned(const char * k, IInterfacePtr a) :
      MappingStringTo<IInterfacePtr,IInterfacePtr>(k,a) { }
  ~MappingStringToOwned() { ::Release(val); }
};

typedef MapStringTo<IInterfacePtr, IInterfacePtr, MappingStringToOwned> MapStringToOwned;

class jlib_decl ChildMap : public SuperHashTableOf<IPropertyTree, constcharptr>
{
public: 
    IMPLEMENT_SUPERHASHTABLEOF_REF_FIND(IPropertyTree, constcharptr);

    ChildMap(bool _nocase) : SuperHashTableOf<IPropertyTree, constcharptr>(4), nocase(_nocase)
    { 
    }
    ~ChildMap() 
    { 
        kill(); 
    }

//
    virtual bool set(const char *key, IPropertyTree *tree)
    {
        return SuperHashTableOf<IPropertyTree, constcharptr>::replace(* tree);
    }

    virtual bool replace(const char *key, IPropertyTree *tree) // provides different semantics, used if element being replaced is not to be treated as deleted.
    {
        return SuperHashTableOf<IPropertyTree, constcharptr>::replace(* tree);
    }

    virtual IPropertyTree *query(const char *key)
    {
        return find(*key);
    }

    virtual bool remove(const char *key)
    {
        return SuperHashTableOf<IPropertyTree, constcharptr>::remove(key);
    }

    virtual bool removeExact(IPropertyTree *child)
    {
        return SuperHashTableOf<IPropertyTree, constcharptr>::removeExact(child);
    }

public:
// SuperHashTable definitions
    virtual void onAdd(void *) {}

    virtual void onRemove(void *e)
    {
        IPropertyTree &elem= *(IPropertyTree *)e;       
        elem.Release();
    }

    virtual unsigned getHashFromElement(const void *e) const;
    virtual unsigned getHashFromFindParam(const void *fp) const
    {
        const char *name = (const char *)fp;
        if (nocase)
            return hashnc((const unsigned char *)name, (size32_t)strlen(name), 0);
        else
            return hashc((const unsigned char *)name, (size32_t)strlen(name), 0);
    }

    virtual const void *getFindParam(const void *e) const
    {
        const IPropertyTree &elem=*(const IPropertyTree *)e;
        return (void *)elem.queryName();
    }

    virtual bool matchesFindParam(const void *e, const void *fp, unsigned fphash) const
    {
        if (nocase)
            return (0 == stricmp(((IPropertyTree *)e)->queryName(), (const char *)fp));
        else
            return (0 == strcmp(((IPropertyTree *)e)->queryName(), (const char *)fp));
    }
private:
    bool nocase;
};


// http://www.w3.org/TR/REC-xml#xml-names
static const char *validStartChrs = ":_";
inline static bool isValidXPathStartChr(char c)
{
    return ('\0' != c && (isalpha(c) || strchr(validStartChrs, c)));
}

static const char *validChrs = ":_.-";
inline static bool isValidXPathChr(char c)
{
    return ('\0' != c && (isalnum(c) || strchr(validChrs, c)));
}

inline static bool isAttribute(const char *xpath) { return (xpath && *xpath == '@'); }

jlib_decl const char *splitXPathUQ(const char *xpath, StringBuffer &path);
jlib_decl const char *queryHead(const char *xpath, StringBuffer &head);
jlib_decl const char *queryNextUnquoted(const char *str, char c);

interface IPTArrayValue
{
    virtual ~IPTArrayValue() { }

    virtual bool isArray() const = 0;
    virtual bool isCompressed() const = 0;
    virtual const void *queryValue() const = 0;
    virtual MemoryBuffer &getValue(MemoryBuffer &tgt, bool binary) const = 0;
    virtual StringBuffer &getValue(StringBuffer &tgt, bool binary) const = 0;
    virtual size32_t queryValueSize() const = 0;
    virtual IPropertyTree *queryElement(unsigned idx) const = 0;
    virtual void addElement(IPropertyTree *e) = 0;
    virtual void setElement(unsigned idx, IPropertyTree *e) = 0;
    virtual void removeElement(unsigned idx) = 0;
    virtual unsigned elements() const = 0;
    virtual const void *queryValueRaw() const = 0;
    virtual unsigned queryValueRawSize() const = 0;

    virtual void serialize(MemoryBuffer &tgt) = 0;
    virtual void deserialize(MemoryBuffer &src) = 0;
};

class CPTArray : implements IPTArrayValue, private Array
{
public:
    virtual bool isArray() const { return true; }
    virtual bool isCompressed() const { return false; }
    virtual const void *queryValue() const { UNIMPLEMENTED; }
    virtual MemoryBuffer &getValue(MemoryBuffer &tgt, bool binary) const { UNIMPLEMENTED; }
    virtual StringBuffer &getValue(StringBuffer &tgt, bool binary) const { UNIMPLEMENTED; }
    virtual size32_t queryValueSize() const { UNIMPLEMENTED; }
    virtual IPropertyTree *queryElement(unsigned idx) const { return (idx<ordinality()) ? &((IPropertyTree &)item(idx)) : NULL; }
    virtual void addElement(IPropertyTree *tree) { append(*tree); }
    virtual void setElement(unsigned idx, IPropertyTree *tree) { add(*tree, idx); }
    virtual void removeElement(unsigned idx) { remove(idx); }
    virtual unsigned elements() const { return ordinality(); }
    virtual const void *queryValueRaw() const { UNIMPLEMENTED; return NULL; }
    virtual unsigned queryValueRawSize() const { UNIMPLEMENTED; return 0; }

// serializable
    virtual void serialize(MemoryBuffer &tgt) { UNIMPLEMENTED; }
    virtual void deserialize(MemoryBuffer &src) { UNIMPLEMENTED; }
};


class jlib_decl CPTValue : implements IPTArrayValue, private MemoryAttr
{
public:
    CPTValue(MemoryBuffer &src)
    { 
        deserialize(src); 
    }
    CPTValue(size32_t size, const void *data, bool binary=false, bool raw=false, bool compressed=false);

    virtual bool isArray() const { return false; }
    virtual bool isCompressed() const { return compressed; }
    virtual const void *queryValue() const;
    virtual MemoryBuffer &getValue(MemoryBuffer &tgt, bool binary) const;
    virtual StringBuffer &getValue(StringBuffer &tgt, bool binary) const;
    virtual size32_t queryValueSize() const;
    virtual IPropertyTree *queryElement(unsigned idx) const { UNIMPLEMENTED; return NULL; }
    virtual void addElement(IPropertyTree *tree) { UNIMPLEMENTED; }
    virtual void setElement(unsigned idx, IPropertyTree *tree) { UNIMPLEMENTED; }
    virtual void removeElement(unsigned idx) { UNIMPLEMENTED; }
    virtual unsigned elements() const {  UNIMPLEMENTED; return (unsigned)-1; }
    virtual const void *queryValueRaw() const { return get(); }
    virtual unsigned queryValueRawSize() const { return length(); }

// serilizable
    virtual void serialize(MemoryBuffer &tgt);
    virtual void deserialize(MemoryBuffer &src);

private:
    mutable bool compressed;
};

// NB PtFlag_Ext6 - used by SDS
enum PtFlags { PtFlag_NoCase=1, PtFlag_Binary=2, PtFlag_Ext1=4, PtFlag_Ext2=8, PtFlag_Ext3=16, PtFlag_Ext4=32, PtFlag_Ext5=64, PtFlag_Ext6=128 };
#define PtFlagTst(fs, f) (0!=(fs&(f)))
#define PtFlagSet(fs, f) (fs |= (f))
#define PtFlagClr(fs, f) (fs &= (~f))

struct AttrStr
{
    unsigned hash;
    unsigned short linkcount;
    char str[1];
    const char *get() const { return str; }
};

struct AttrValue
{
    AttrStr *key;
    AttrStr *value;
};

#define AM_NOCASE_FLAG (0x8000)
#define AM_NOCASE_MASK (0x7fff)

class jlib_decl AttrMap
{
    AttrValue *attrs;
    unsigned short numattrs;    // use top bit for nocase flag
    static AttrValue **freelist; // entry 0 not used
    static unsigned freelistmax; 
    static CLargeMemoryAllocator freeallocator;

    AttrValue *newArray(unsigned n);
    void freeArray(AttrValue *a,unsigned n);

public:
    AttrMap()
    { 
        attrs = NULL;
        numattrs = 0;
    }
    ~AttrMap() 
    { 
        kill();
    }
    inline unsigned count() const
    { 
        return numattrs&AM_NOCASE_MASK; 
    }
    inline AttrValue *item(unsigned i) const
    { 
        return attrs+i; 
    }
    void setNoCase(bool nc) 
    { 
        if (nc) 
            numattrs |= AM_NOCASE_FLAG; 
        else 
            numattrs &= AM_NOCASE_MASK; 
    }
    inline bool isNoCase() const 
    { 
        return (numattrs&AM_NOCASE_FLAG)!=0; 
    }
    void kill();
    void set(const char *key, const char *val);
    const char *find(const char *key) const;
    bool remove(const char *key);
    void swap(AttrMap &other);
    static inline void killfreelist()
    {
        free(freelist);
        freelist = NULL;
    }
};


class jlib_decl PTree : public CInterface, implements IPropertyTree
{
friend class SingleIdIterator;
friend class PTLocalIteratorBase;
friend class PTIdMatchIterator;

public:
    IMPLEMENT_IINTERFACE;
    virtual bool IsShared() const { return CInterface::IsShared(); }

    PTree(MemoryBuffer &mb);
    PTree(const char *_name=NULL, bool _nocase=false, IPTArrayValue *_value=NULL, ChildMap *_children=NULL);
    ~PTree();

    IPropertyTree *queryParent() { return parent; }
    IPropertyTree *queryChild(unsigned index);
    ChildMap *queryChildren() { return children; }
    aindex_t findChild(IPropertyTree *child, bool remove=false);
    inline bool isnocase() const { return PtFlagTst(flags, PtFlag_NoCase); }
    const PtFlags queryFlags() const { return (PtFlags) flags; }
public:
    void serializeCutOff(MemoryBuffer &tgt, int cutoff=-1, int depth=0);
    void serializeAttributes(MemoryBuffer &tgt);
    virtual void serializeSelf(MemoryBuffer &tgt);
    virtual void deserializeSelf(MemoryBuffer &src);
    virtual void createChildMap(bool caseInsensitive=true) { children = new ChildMap(caseInsensitive); }


    HashKeyElement *queryKey() { return name; }
    void setName(const char *_name);
    IPropertyTree *clone(IPropertyTree &srcTree, bool self=false, bool sub=true);
    void clone(IPropertyTree &srcTree, IPropertyTree &dstTree, bool sub=true);
    inline void setParent(IPropertyTree *_parent) { parent = _parent; }
    IPropertyTree *queryCreateBranch(IPropertyTree *branch, const char *prop, bool *existing=NULL);
    IPropertyTree *splitBranchProp(const char *xpath, const char *&_prop, bool error=false);
    IPTArrayValue *queryValue() { return value; }
    IPTArrayValue *detachValue() { IPTArrayValue *v = value; value = NULL; return v; }
    void setValue(IPTArrayValue *_value, bool binary) { if (value) delete value; value = _value; if (binary) PtFlagSet(flags, PtFlag_Binary); }
    bool checkPattern(const char *&xxpath) const;
    void clear();

// IPropertyTree impl.
    virtual bool hasProp(const char * xpath) const;
    virtual bool isBinary(const char *xpath=NULL) const;
    virtual bool isCompressed(const char *xpath=NULL) const;
    virtual bool renameProp(const char *xpath, const char *newName);
    virtual bool renameTree(IPropertyTree *tree, const char *newName);
    virtual const char *queryProp(const char *xpath) const;
    virtual bool getProp(const char *xpath, StringBuffer &ret) const;
    virtual void setProp(const char *xpath, const char *val);
    virtual void addProp(const char *xpath, const char *val);
    virtual void appendProp(const char *xpath, const char *val);
    virtual bool getPropBool(const char *xpath, bool dft=false) const;
    virtual void setPropBool(const char *xpath, bool val) { setPropInt(xpath, val); }
    virtual void addPropBool(const char *xpath, bool val) { addPropInt(xpath, val); }
    virtual __int64 getPropInt64(const char *xpath, __int64 dft=0) const;
    virtual void setPropInt64(const char * xpath, __int64 val);
    virtual void addPropInt64(const char *xpath, __int64 val);
    virtual int getPropInt(const char *xpath, int dft=0) const;
    virtual void setPropInt(const char *xpath, int val);
    virtual void addPropInt(const char *xpath, int val);
    virtual bool getPropBin(const char * xpath, MemoryBuffer &ret) const;
    virtual void setPropBin(const char * xpath, size32_t size, const void *data);
    virtual void appendPropBin(const char *xpath, size32_t size, const void *data);
    virtual void addPropBin(const char *xpath, size32_t size, const void *data);
    virtual IPropertyTree *getPropTree(const char *xpath) const;
    virtual IPropertyTree *queryPropTree(const char *xpath) const;
    virtual IPropertyTree *getBranch(const char *xpath) const { return LINK(queryBranch(xpath)); }
    virtual IPropertyTree *queryBranch(const char *xpath) const { return queryPropTree(xpath); }
    virtual IPropertyTree *setPropTree(const char *xpath, IPropertyTree *val);
    virtual IPropertyTree *addPropTree(const char *xpath, IPropertyTree *val);
    virtual bool removeTree(IPropertyTree *child);
    virtual bool removeProp(const char *xpath);
    virtual aindex_t queryChildIndex(IPropertyTree *child);
    virtual const char *queryName() const;
    virtual StringBuffer &getName(StringBuffer &ret) const;
    virtual IAttributeIterator *getAttributes(bool sorted=false) const;
    virtual IPropertyTreeIterator *getElements(const char *xpath, IPTIteratorCodes flags = iptiter_null) const;
    virtual void localizeElements(const char *xpath, bool allTail=false);
    virtual bool hasChildren() const { return children && children->count()?true:false; }
    virtual unsigned numUniq() { return checkChildren()?children->count():0; }  
    virtual unsigned numChildren();
    virtual bool isCaseInsensitive() { return isnocase(); }
// serializable impl.
    virtual void serialize(MemoryBuffer &tgt);
    virtual void deserialize(MemoryBuffer &src);
    const AttrMap &queryAttributes() const { return attributes; }
    
protected:
    virtual ChildMap *checkChildren() const;
    virtual bool isEquivalent(IPropertyTree *tree) { return (NULL != QUERYINTERFACE(tree, PTree)); }
    virtual void setLocal(size32_t l, const void *data, bool binary=false);
    virtual void appendLocal(size32_t l, const void *data, bool binary=false);
    virtual void setAttr(const char *attr, const char *val);
    virtual bool removeAttr(const char *attr);
    virtual void addingNewElement(IPropertyTree &child, int pos) { }
    virtual void removingElement(IPropertyTree *tree, unsigned pos) { }
    virtual IPropertyTree *create(const char *name=NULL, bool nocase=false, IPTArrayValue *value=NULL, ChildMap *children=NULL, bool existing=false) = 0;
    virtual IPropertyTree *create(MemoryBuffer &mb) = 0;
    virtual IPropertyTree *ownPTree(IPropertyTree *tree);

    aindex_t getChildMatchPos(const char *xpath);

private:
    void init();
    void addLocal(size32_t l, const void *data, bool binary=false, int pos=-1);
    void resolveParentChild(const char *xpath, IPropertyTree *&parent, IPropertyTree *&child, StringAttr &path, StringAttr &qualifier);
    void replaceSelf(IPropertyTree *val);

protected: // data
    IPropertyTree *parent; // ! currently only used if tree embedded into array, used to locate position.

    HashKeyElement *name;
    ChildMap *children;
    IPTArrayValue *value;
    AttrMap attributes;
    byte flags;
};

jlib_decl IPropertyTree *createPropBranch(IPropertyTree *tree, const char *xpath, bool createIntermediates=false, IPropertyTree **created=NULL, IPropertyTree **createdParent=NULL);

class LocalPTree : public PTree
{
public:
    LocalPTree(const char *name=NULL, bool nocase=false, IPTArrayValue *value=NULL, ChildMap *children=NULL)
        : PTree(name, nocase, value, children) { }

    virtual bool isEquivalent(IPropertyTree *tree) { return (NULL != QUERYINTERFACE(tree, LocalPTree)); }
    
    virtual IPropertyTree *create(const char *name=NULL, bool nocase=false, IPTArrayValue *value=NULL, ChildMap *children=NULL, bool existing=false)
    {
        return new LocalPTree(name, nocase, value, children);
    }

    virtual IPropertyTree *create(MemoryBuffer &mb)
    {
        IPropertyTree *tree = new LocalPTree();
        tree->deserialize(mb);
        return tree;
    }
};

class PTree;
class SingleIdIterator : public CInterface, implements IPropertyTreeIterator
{
public:
    IMPLEMENT_IINTERFACE;

    SingleIdIterator(const PTree &_tree, unsigned pos=1, unsigned _many=(unsigned)-1);
    ~SingleIdIterator();
    void setCurrent(unsigned pos);

// IPropertyTreeIterator
    virtual bool first();
    virtual bool next();
    virtual bool isValid();
    virtual IPropertyTree & query() { return * current; }

private:
    unsigned many, count, whichNext, start;
    IPropertyTree *current;
    const PTree &tree;
};


class PTLocalIteratorBase : public CInterface, implements IPropertyTreeIterator
{
public:
    IMPLEMENT_IINTERFACE;

    PTLocalIteratorBase(const PTree *tree, const char *_id, bool _nocase, bool sort);

    ~PTLocalIteratorBase();
    virtual bool match() = 0;

// IPropertyTreeIterator
    virtual bool first();
    virtual bool next();
    virtual bool isValid();
    virtual IPropertyTree & query() { return iter->query(); }

protected:
    IPropertyTreeIterator *baseIter;
    StringAttr id;
    bool nocase, sort;
private:
    const PTree *tree;
    IPropertyTreeIterator *iter;
    IPropertyTree *current;
    bool _next();
};

class PTIdMatchIterator : public PTLocalIteratorBase
{
public:
    IMPLEMENT_IINTERFACE;

    PTIdMatchIterator(const PTree *tree, const char *id, bool nocase, bool sort) : PTLocalIteratorBase(tree, id, nocase, sort) { }

    virtual bool match();
};

class StackElement;

class PTStackIterator : public CInterface, implements IPropertyTreeIterator
{
public:
    IMPLEMENT_IINTERFACE;

    PTStackIterator(IPropertyTreeIterator *_iter, const char *_xpath);
    ~PTStackIterator();

// IPropertyTreeIterator
    virtual bool first();
    virtual bool isValid();
    virtual bool next();
    virtual IPropertyTree & query();

private:
    void setIterator(IPropertyTreeIterator *iter);
    void pushToStack(IPropertyTreeIterator *iter, const char *xpath);
    IPropertyTreeIterator *popFromStack(StringAttr &path);

private: // data
    IPropertyTreeIterator *rootIter, *iter;
    const char *xxpath;
    IPropertyTree *current;
    StringAttr xpath, stackPath;
    unsigned stacklen;
    unsigned stackmax;
    StackElement *stack;
};

class CPTreeMaker : public CInterface, implements IPTreeMaker
{
    IPropertyTree *root;
    ICopyArrayOf<IPropertyTree> ptreeStack;
    bool caseInsensitive, rootProvided, noRoot;
    IPTreeNodeCreator *nodeCreator;
    class CDefaultNodeCreator : public CInterface, implements IPTreeNodeCreator
    {
        bool caseInsensitive;
    public:
        IMPLEMENT_IINTERFACE;

        CDefaultNodeCreator(bool _caseInsensitive) : caseInsensitive(_caseInsensitive) { }

        virtual IPropertyTree *create(const char *tag) { return createPTree(tag, caseInsensitive); }
    };
protected:
    IPropertyTree *currentNode;
public:
    IMPLEMENT_IINTERFACE;

    CPTreeMaker(bool _caseInsensitive, IPTreeNodeCreator *_nodeCreator=NULL, IPropertyTree *_root=NULL, bool _noRoot=false) : caseInsensitive(_caseInsensitive), noRoot(_noRoot)
    {
        if (_nodeCreator)
            nodeCreator = LINK(_nodeCreator);
        else
            nodeCreator = new CDefaultNodeCreator(caseInsensitive);
        if (_root)
        { 
            root = LINK(_root);
            rootProvided = true;
        }
        else
        {
            root = NULL;    
            rootProvided = false;
        }
        reset();
    }
    ~CPTreeMaker()
    {
        ::Release(nodeCreator);
        ::Release(root);
    }

// IPTreeMaker
    virtual void beginNode(const char *tag, offset_t startOffset)
    {
        if (rootProvided)
        {
            currentNode = root;
            rootProvided = false;
        }
        else
        {
            IPropertyTree *parent = currentNode;
            if (!root)
            {
                currentNode = nodeCreator->create(tag);
                root = currentNode;
            }
            else
                currentNode = nodeCreator->create(NULL);
            if (parent)
                parent->addPropTree(tag, currentNode);
            else if (noRoot)
                root->addPropTree(tag, currentNode);
        }
        ptreeStack.append(*currentNode);
    }
    virtual void newAttribute(const char *name, const char *value)
    {
        currentNode->setProp(name, value);
    }
    virtual void beginNodeContent(const char *name) { }
    virtual void endNode(const char *tag, unsigned length, const void *value, bool binary, offset_t endOffset)
    {
        if (binary)
            currentNode->setPropBin(NULL, length, value);
        else
            currentNode->setProp(NULL, (const char *)value);
        unsigned c = ptreeStack.ordinality();
        if (c==1 && !noRoot && currentNode != root) 
            ::Release(currentNode);
        ptreeStack.pop();
        currentNode = (c>1) ? &ptreeStack.tos() : NULL;
    }
    virtual IPropertyTree *queryRoot() { return root; }
    virtual IPropertyTree *queryCurrentNode() { return currentNode; }
    virtual void reset()
    {
        if (!rootProvided)
        {
            ::Release(root);
            if (noRoot)
                root = nodeCreator->create("__NoRoot__");
            else
                root = NULL;
        }
        currentNode = NULL;
    }
};

#ifdef __64BIT__
#pragma pack(pop)   
#endif


#endif