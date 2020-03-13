#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <cassert>
#include <tr1/memory>
#include <algorithm>

#include <QtCore>


struct ChangedEmittor : public QObject
{
  Q_OBJECT

signals:
  void Changed();
};

template<typename T>
class VectorData : public ChangedEmittor
{
public:
  typedef std::shared_ptr<T>              ItemPtr;
  typedef std::vector<ItemPtr> Vector;
  typedef typename Vector::iterator       Iterator;
  typedef typename Vector::const_iterator ConstIterator;

public:
  void   Append(T* t){ vector_.push_back(ItemPtr(t)); OnChanged(); }
  void   Clear(){ vector_.clear(); OnChanged(); }
  size_t Size() const { return vector_.size(); }

  Iterator begin(){ return vector_.begin(); }
  Iterator end()  { return vector_.end(); }
  ConstIterator cbegin() const { return vector_.cbegin(); }
  ConstIterator cend() const   { return vector_.cend(); }

  // compatibility
  size_t size() const { return vector_.size(); }

        T& operator[](size_t index)       { return *vector_[index]; }
  const T& operator[](size_t index) const { return *vector_[index]; }

private:
  void OnChanged() { emit Changed(); }

private:
  Vector         vector_;
};


// specialize this template, no implementation here -> compiler errors
template<typename T>
struct VectorModelTrait
{
  static QString GetName(const T& t);
};


class ListModelWrapper : public QAbstractListModel
{
Q_OBJECT

public:
  ListModelWrapper(){}

public slots:
  void DoReset(){ reset(); }
};

template<typename Item,
	 typename Vector = VectorData<Item>, 
	 typename ItemTrait = VectorModelTrait<Item>>
class VectorModelWrapper : public ListModelWrapper
{
public:
  VectorModelWrapper() : vector_(0) {}
  VectorModelWrapper(const Vector* vec) { SetVectorData(vec); }

  // interface
  void SetVectorData(const Vector* vec){ assert(vec); vector_ = vec;
    CreateConnections();
    // notify listeners
    DoReset();
  }
  
  // implementing pure virtuals
  int      rowCount ( const QModelIndex& parent = QModelIndex() ) const;
  QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;

private:
  void CreateConnections(){ 
    connect( vector_, SIGNAL(Changed()),
	     this, SLOT(DoReset()) ); }

private:
  const Vector* vector_;
};

template<typename Item, typename Vector, typename ItemTrait>
int
VectorModelWrapper<Item, Vector, ItemTrait>::rowCount( const QModelIndex& parent) const
{
  int size = 0;
  if (! vector_ == 0 || parent.isValid())
    size =  vector_->size();

  return size;
}

template<typename Item, typename Vector, typename ItemTrait>
QVariant
VectorModelWrapper<Item, Vector, ItemTrait>::data ( const QModelIndex& index, int role ) const
{
  // list-based model: only one column
  assert(index.column() == 0);

  if (role == Qt::DisplayRole){
    return QVariant( ItemTrait::GetName((*vector_)[index.row()]) );
  } else 
    return QVariant();
}


template<typename Table>
class TableModelWrapper : public QAbstractTableModel
{
public:
  TableModelWrapper() : table_(0) {}
  TableModelWrapper(const Table* table) : table_(table) {}

  // interface
  void SetTableData(const Table* table){ assert(table); table_ = table;
    // notify listeners
    reset();
  }
  
  // implementing pure virtuals
  int      rowCount ( const QModelIndex& parent = QModelIndex() ) const;
  int      columnCount ( const QModelIndex& parent = QModelIndex() ) const;
  QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
  QVariant headerData ( int section, Qt::Orientation orientation
			, int role = Qt::DisplayRole ) const;

private:
  const Table* table_;
};

template<typename Table>
QVariant
TableModelWrapper<Table>::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
    return QVariant();

  return QVariant( table_->GetHeader(section) );
}

template<typename Table>
int
TableModelWrapper<Table>::rowCount( const QModelIndex& parent) const
{
  if (table_ == 0)
    return 0;

  if(parent.isValid())
    return 0;
  else
    return table_->GetNumRows();
}


template<typename Table>
int
TableModelWrapper<Table>::columnCount( const QModelIndex& parent) const
{
  if (table_ == 0)
    return 0;

  if(parent.isValid())
    return 0;
  else
    return table_->GetNumCols();
}

template<typename Table>
QVariant
TableModelWrapper<Table>::data ( const QModelIndex& index, int role ) const
{
  if (role == Qt::DisplayRole){
    return QVariant( table_->Get(index.row(), index.column()) );
  } else 
    return QVariant();
}

template<typename T, typename Type>
struct Compare
{
  Compare(int icol) : col(icol) {}
  bool operator()(const T& v1, const T& v2) const;

private:
  int col;
};

template<typename T>
struct Compare<T, int>
{
  Compare(int icol) : col(icol) {}
  bool operator()(const T& v1, const T& v2) const
  { return v1.GetField(col).toInt() < v2.GetField(col).toInt(); }

private:
  int col;
};

template<typename T>
class TableWrapper
{
public:
  typedef std::vector<T> Vector;
  typedef typename Vector::iterator       Iterator;
  typedef typename Vector::const_iterator ConstIterator;  

public:
  size_t GetNumRows() const { return list_.size(); }
  size_t GetNumCols() const { return T::GetNumInformationFields() ; }  

  void     Clear(){ list_.clear(); }
  T&       Get(size_t row) { assert(IsValidIndex(row)); return list_[row]; }
  const T& Get(size_t row) const { assert(IsValidIndex(row)); return list_[row]; }
  QVariant Get(size_t row, size_t col) const { assert(IsValidIndex(row)); 
    return list_[row].GetField(col); }
  QVariant GetHeader(size_t col) const { return T::GetFieldName(col); }
  void Append(const T& t){ list_.push_back(t); }
  bool IsValidIndex(int index) const
  { return index >= 0 && static_cast<size_t>(index) < GetNumRows(); }
  
  template<typename Type>
  void SortByColumn(int col)
  { std::sort(list_.begin(), list_.end(), Compare<T, Type>(col));  }

  Iterator begin(){ return list_.begin(); }
  Iterator end()  { return list_.end(); }
  ConstIterator cbegin() const { return list_.cbegin(); }
  ConstIterator cend() const   { return list_.cend(); }

private:
  Vector list_;
};


#endif // __MODEL_HPP__
