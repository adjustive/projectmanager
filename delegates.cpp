#include "delegates.h"
#include <QSpinBox>
#include <QComboBox>
#include <QDateEdit>

SpinBoxDelegate::SpinBoxDelegate(int min, int max, QObject *parent): QItemDelegate(parent)
{
    this->min = min;
    this->max = max;
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
 {
     QSpinBox *editor = new QSpinBox(parent);
     editor->setMinimum(this->min);
     editor->setMaximum(this->max);
     return editor;
 }

void SpinBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::EditRole).toInt();
    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
 {
     QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
     spinBox->interpretText();
     int value = spinBox->value();

     model->setData(index, value, Qt::EditRole);
 }

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }


///////////////// ComboBox Delegate ////////////////////


BoolDelegate::BoolDelegate(  QObject *parent): QItemDelegate(parent)
{
}

BoolDelegate::~BoolDelegate()
{
}

QWidget *BoolDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
 {
     QComboBox *editor = new QComboBox(parent);
     editor->insertItem(0,"Yes");
     editor->insertItem(1,"No");
     return editor;
 }

void BoolDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    bool value = index.model()->data(index, Qt::EditRole).toBool();
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    comboBox->setCurrentIndex(value?0:1);
}

void BoolDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
 {
     QComboBox *comboBox = static_cast<QComboBox*>(editor);

     bool value = comboBox->currentText() == "Yes";

     model->setData(index, value, Qt::EditRole);
 }

void BoolDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }



///////////////// Date Delegate ////////////////////


DateDelegate::DateDelegate(  QObject *parent): QItemDelegate(parent)
{
}

DateDelegate::~DateDelegate()
{
}

QWidget *DateDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
 {
     QDateEdit *editor = new QDateEdit(parent);
     return editor;
 }

void DateDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QDate value = index.model()->data(index, Qt::EditRole).toDate();
    QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);
    dateEdit->setDate(value);
}

void DateDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
 {
     QDateEdit *dateEdit = static_cast<QDateEdit*>(editor);

     QDate value = dateEdit->date();

     model->setData(index, value, Qt::EditRole);
 }

void DateDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
 {
     editor->setGeometry(option.rect);
 }




