#include "memoryapplication.h"
#include "ui_memoryapplication.h"

#include <QStringListModel>
#include <QList>
#include <QMessageBox>
#include "memoryitem.h"

QStringList * memoryStrings;
QStringListModel * memoryStringsModel;

QList<memoryItem> itemsInMemory;

int nextProcessNumber;

MemoryApplication::MemoryApplication(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MemoryApplication)
{
    ui->setupUi(this);

    //link lists to models and models to list views
    memoryStrings = new QStringList();
    memoryStringsModel = new QStringListModel(*memoryStrings, NULL);
    ui->lstMemory->setModel(memoryStringsModel);

    nextProcessNumber = 2;

    //set the default memory block, all free
    memoryItem EmptyBlock("Free Space", 16, true);
    itemsInMemory.append(EmptyBlock);

    UpdateMemoryList();
}

MemoryApplication::~MemoryApplication()
{
    delete ui;
}

void MemoryApplication::on_btnNewProcess_clicked()
{
  //add the new process to the list
    //use first fit to get the process into memory.
    //if we reach the end then we need to garbage collect.
      //create new memory item
        memoryItem newMemoryBlock(ui->txtNewProcess->text(), ui->intMemNeeded->value(), false);
        bool memItemInList = false;
        bool toBeRemoved = false;
        int toBeRemovedIndex = 0;
      //iterate through list until there is a free block that is larger than this process
        for (int i = 0; i < itemsInMemory.size(); i++)
        {
            qInfo(itemsInMemory[i].Display().toLatin1());
            if (itemsInMemory[i].isFree && itemsInMemory[i].memoryOccupying >= newMemoryBlock.memoryOccupying)
            {
                qInfo("\tis free and big enough");
                //its free and bigger, we can dump it in here and move on
                //deduct the new items size from this
                if(itemsInMemory[i].memoryOccupying == newMemoryBlock.memoryOccupying)
                {
                    qInfo("\tare same size, removing");
                    //cant be removed inside this loop, flag for removal
                    toBeRemoved = true;
                    toBeRemovedIndex = i;
                }
                else
                {
                    qInfo("\tis bigger, subtracting");
                    //shrink the free memory block
                    itemsInMemory[i].setMemoryOccupying( itemsInMemory[i].memoryOccupying - newMemoryBlock.memoryOccupying);
                    qInfo("\t" + QString::number(itemsInMemory[i].memoryOccupying).toLatin1() + " is new size");
                }

                //then put it in the list
                itemsInMemory.insert(itemsInMemory.indexOf(itemsInMemory[i]),newMemoryBlock);
                memItemInList = true;

                //done with this list for now
                break;
            }
        }
      //see if the free space needs to be removed, if so do it
        if (toBeRemoved)
        {
            qInfo("\tremoving...");
            //add one because our new memory was inserted at the location
            itemsInMemory.removeAt(toBeRemovedIndex + 1);
        }
      //see if we got it in, if not garbage collect
        if (!memItemInList)
        {
            qInfo("not in the list, to big or needs garbage collection");
            //in order to garbage collect we shuffle the memory contents to place all free memory in one block

            //if its free space remove it and add up the total, we will dump all the free space at the back
            int freeSpaceTotal = 0;
            int i = 0;
            while (i < itemsInMemory.size())
            {
                if (itemsInMemory[i].isFree)
                {
                    //add it, delete it, don't increment i
                    freeSpaceTotal += itemsInMemory[i].memoryOccupying;
                    itemsInMemory.removeAt(i);
                }
                else
                {
                    //increment i becuase we didn't remove anything
                    i++;
                }
            }
            //if there is space make new free space block
            if (freeSpaceTotal > 0)
            {
                memoryItem newFreeSpaceBlock("Free Space", freeSpaceTotal, true);
                itemsInMemory.append(newFreeSpaceBlock);
            }

            //if there is not enough free space then throw an error to the user, if there is put it in
            if (newMemoryBlock.memoryOccupying <= freeSpaceTotal)
            {
                //we have enogh space
                //we know the free space is at the end (index = size - 1) so this goes there
                itemsInMemory.insert(itemsInMemory.size()-1,newMemoryBlock);
                //if we use all the free space then we need to kill the free space, if not we need to shrink it
                if (newMemoryBlock.memoryOccupying == freeSpaceTotal)
                {
                    //we used it all, just remove it
                    itemsInMemory.pop_back();
                }
                else
                {
                    //there is some left so we need to calculate that
                    itemsInMemory.last().setMemoryOccupying(freeSpaceTotal - newMemoryBlock.memoryOccupying);
                }
            }
            else
            {
                //we do not have enough space to add this process but the free mem block is already added, just neet to tell the user
                UpdateMemoryList();

                QMessageBox msgBox;
                msgBox.setText("After compaction There is not enough free memory to add this process.");
                msgBox.exec();
            }

        }
      // update the list box
        UpdateMemoryList();

  //increment the default process name in the box
    ui->txtNewProcess->setText(QString("Process %1").arg(nextProcessNumber++));
}

void MemoryApplication::on_btnTerminate_clicked()
{
    // make list of selected items from listview on form: Should only be one, I am not sure how to multi select
    QModelIndexList ListOfItemsOnForm = ui->lstMemory->selectionModel()->selectedIndexes();

    foreach (const QModelIndex &selectedIndex, ListOfItemsOnForm)
    {
        // check it against the items in blocked list, if found move it to ready list
        foreach (memoryItem m, itemsInMemory)
        {
            QRegExp rx("*" + m.Display());
            rx.setPatternSyntax(QRegExp::Wildcard);
            //QRegExp rx("*.txt");
            //rx.exactMatch("README.txt");        // returns true
            //rx.exactMatch("welcome.txt.bak");   // returns false
            if (rx.exactMatch(selectedIndex.data(Qt::DisplayRole).toString()))
            {
                //and change it to free space
                itemsInMemory[itemsInMemory.indexOf(m)].name = "Free Space";
                itemsInMemory[itemsInMemory.indexOf(m)].isFree = true;
            }
        }
    }

    //if two free spaces are next to eachother combine them
    int i = 0;
    while (i < itemsInMemory.size())
    {
        if (itemsInMemory[i].isFree)
        {
            //if this one and the next one is free combine them, check size to not get out of range exceptions
            if (i+1 >= itemsInMemory.size())
            {
                //this is the last item go ahead and increment i and we are done
                i++;
            }
            else
            {
                //not the last item so we can check the next one
                if (itemsInMemory[i+1].isFree)
                {
                    //this one is free so is the next, remove the next adding its space to this one
                    itemsInMemory[i].memoryOccupying += itemsInMemory[i+1].memoryOccupying;
                    itemsInMemory.removeAt(i+1);
                }
                else
                {
                    //this free block is isolated, move along
                    i++;
                }
            }
        }
        else
        {
            //increment i becuase we didn't remove anything
            i++;
        }
    }


    //update everything
    UpdateMemoryList();
}

void MemoryApplication::UpdateMemoryList()
{
    memoryStrings->clear();
    int itemNumber = 0;
    foreach (memoryItem m, itemsInMemory) {
        memoryStrings->append(QString::number(itemNumber) + "\t" + m.Display());
        itemNumber += m.memoryOccupying;
    }
    memoryStringsModel->setStringList(*memoryStrings);
}
