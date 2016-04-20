template<class T, class K>
class OSHashTable {
public:
    OSHashTable( UInt32 size )
    {
        fHashTable = new ( T*[size] );
        Assert( fHashTable );
        memset( fHashTable, 0, sizeof(T*) * size );
        fSize = size;
        // Determine whether the hash size is a power of 2
        // if not set the mask to zero, otherwise we can
        // use the mask which is faster for ComputeIndex
        fMask = fSize - 1;
        if ((fMask & fSize) != 0)
            fMask = 0;
        fNumEntries = 0;
    }
    ~OSHashTable()
    {
        delete [] fHashTable;
    }
    void Add( T* entry ) {
        Assert( entry->fNextHashEntry == NULL );
        K key( entry );
        UInt32 theIndex = ComputeIndex( key.GetHashKey() );
        entry->fNextHashEntry = fHashTable[ theIndex ];
        fHashTable[ theIndex ] = entry;
        fNumEntries++;
    }
    void Remove( T* entry )
    {
        K key( entry );
        UInt32 theIndex = ComputeIndex( key.GetHashKey() );
        T* elem = fHashTable[ theIndex ];
        T* last = NULL;
        while (elem && elem != entry) {
            last = elem;
            elem = elem->fNextHashEntry;
        }
    
        if ( elem ) // sometimes remove is called 2x ( swap, then un register )
        {
            Assert(elem);
            if (last)
                last->fNextHashEntry = elem->fNextHashEntry;
            else
                fHashTable[ theIndex ] = elem->fNextHashEntry;
            elem->fNextHashEntry = NULL;
            fNumEntries--;
        }
    }
    T* Map( K* key )
    {
        UInt32 theIndex = ComputeIndex( key->GetHashKey() );
        T* elem = fHashTable[ theIndex ];
        while (elem) {
            K elemKey( elem );
            if (elemKey == *key)
                break;
            elem = elem->fNextHashEntry;
        }
        return elem;
    }
    UInt64 GetNumEntries() { return fNumEntries; }
    
    UInt32 GetTableSize() { return fSize; }
    T* GetTableEntry( int i ) { return fHashTable[i]; }

private:
    T** fHashTable;
    UInt32 fSize;
    UInt32 fMask;
    UInt64 fNumEntries;
    
    UInt32 ComputeIndex( UInt32 hashKey )
    {
        if (fMask)
            return( hashKey & fMask );
        else
            return( hashKey % fSize );
    }
};
