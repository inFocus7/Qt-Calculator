#ifndef HISTORY_H
#define HISTORY_H
#include "arithmetics.h"

// Might need to switch to array/vector to access history quickly when user clicks on an equation...
namespace datastore {

class dLL
{
private:
    class node
    {
    public:
        maths::dLL data;
        node * next, * prev;
        explicit node(maths::dLL & x)
        {
            next = nullptr;
            prev = nullptr;
            data = x;
        }
    };
    node * head{nullptr}, * tail{nullptr};
    unsigned int numItems{0}, maxItems{15};

public:

    dLL() = default;

    QString insert(maths::dLL data)
    {
    node * tmp = new node(data);

    if(isFull())
    {
        remove();
    }

    if(head == nullptr)
    {
        head = tmp;
        tail = tmp;
    }
    else
    {
        tail->next = tmp;
        tmp->prev = tail;
        tail = tmp;
    }

    numItems++;

    // Insert latest data into History.
    // Create a new qpushbutton in the scrollarea with parse as text.
    return tail->data.returnEquation();
    }

    unsigned int getNumItems() const
    {
        return numItems;
    }

    unsigned int getMaxItems() const
    {
        return maxItems;
    }

    void remove()
    {
        if(head != tail)
        {
            node * doomed = head;
            head = head->next;
            head->prev = nullptr;
            delete doomed;
        }
        else
        {
            head = nullptr;
        }
        numItems--;
    }

    bool isFull() const
    {
        if(numItems == maxItems)
            return true;
        return false;
    }

    bool isEmpty() const
    {
        if(numItems == 0)
            return true;
        return false;
    }

    void clear()
    {
        while (!isEmpty())
        {
            remove();
        }
        head = nullptr;
    }


    ~dLL()
    {
        while(!isEmpty())
        {
            remove();
        }
    }
};

}
#endif // HISTORY_H
