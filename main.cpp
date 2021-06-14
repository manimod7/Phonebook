#include <iostream>
#include <string.h>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <conio.h>
#define CHARS 200
using namespace std;

struct TrieNode
{
    TrieNode()
    {
        number = "";
        for (int i = 0; i < CHARS; i++) //default constructor
            child[i] = NULL;
    }
    string number;
    TrieNode *child[CHARS];
};
class Trie
{
public:
    Trie();
    ~Trie();
    void insertContact(string s, string number);
    void findContact(string, bool &);
    void searchContact(string);
    bool autoCompleteContact(TrieNode *root, string s, vector<string> &);
    bool isEmptyContact(TrieNode *);
    TrieNode *removeUntil(TrieNode *, string, int);
    void remove(string);

private:
    TrieNode *root;
};
Trie::Trie()
{
    root = new TrieNode();
}
Trie::~Trie()
{
    // Free memory
}
void Trie::insertContact(string contact, string number) //insert contact and number into trie
{
    int i, idx;
    TrieNode *curr = root;
    for (i = 0; i < contact.length(); i++)
    {
        idx = contact[i] - 'A';
        if (curr->child[idx] == NULL)
            curr->child[idx] = new TrieNode();
        curr = curr->child[idx];
    }
    curr->number = number;
}
void Trie::findContact(string contact, bool &bb) //find contact in trie
{
    TrieNode *curr = root;
    int i, idx;
    for (i = 0; i < contact.size(); i++)
    {
        idx = contact[i] - 'A';
        if (curr->child[idx] == NULL)
        {
            bb = false;
            return;
        }
        curr = curr->child[idx];
    }
    if (curr->number == "")
    {
        bb = false;
        return;
    }
    if (curr->number != "")
    {
        bb = true;
        return;
    }
}
bool Trie::autoCompleteContact(TrieNode *temp, string contact, vector<string> &similarContacts) //autocomplete feature
{
    TrieNode *curr = temp;
    if (temp->number != "")
    {
        similarContacts.push_back(contact);
    }
    for (int i = 0; i < CHARS; i++)
    {
        if (curr->child[i])
            autoCompleteContact(curr->child[i], contact + (char)(i + 'A'), similarContacts);
    }
}
void Trie::searchContact(string contact) // search contact in a Phonebook
{
    int i, idx;
    TrieNode *curr = root;
    vector<string> similarContacts;
    for (i = 0; i < contact.length(); i++)
    {
        idx = contact[i] - 'A';
        if (curr->child[idx] == NULL)
        {
            autoCompleteContact(curr, contact.substr(0, i), similarContacts);
            break;
        }
        curr = curr->child[idx];
    }
    if (curr->number == "")
        autoCompleteContact(curr, contact, similarContacts);

    if (curr->number != "")
        cout << "\n\tThe person \"" << contact << "\" was found with contact number \"" << curr->number << "\"";
    else
    {
        cout << "\n\tThe person \"" << contact << "\" was not found in the Phonebook";
        if (similarContacts.size() == 0)
            return;
        // cout << "\n\tAutoComplete feature :";
        // cout<<"\n\tDid you mean? ";
        // for(int i =0;i<similarContacts.size();i++){
        //     cout << "\n\t" <<similarContacts[i];
        // }
    }
}
bool Trie::isEmptyContact(TrieNode *curr)
{
    int i;
    for (i = 0; i < CHARS; i++)
        if (curr->child[i])
            return 0;
    return 1;
}
TrieNode *Trie::removeUntil(TrieNode *curr, string contact, int depth = 0) //remove contacts from the Phonebook Trie
{
    if (depth == contact.size())
    {
        curr->number = "";
        if (isEmptyContact(curr))
        {
            delete (curr);
            curr = NULL;
        }
        return curr;
    }
    int idx = contact[depth] - 'A';
    curr->child[idx] = removeUntil(curr->child[idx], contact, depth + 1);
    if (isEmptyContact(curr) && curr->number == "")
    {
        delete (curr);
        curr = NULL;
    }
    return curr;
}

void Trie::remove(string contact)
{
    removeUntil(root, contact);
    cout << "\n\tThe person \"" << contact << " has been successfully removed from the Phonebook. ";
}

bool loadPhonebook(Trie *trie, string fn)
{

    ifstream contacts;
    ifstream input;
    contacts.open(fn.c_str());
    if (!contacts.is_open())
    {
        cout << "Could not open the Phonebook file" << endl;
        return false;
    }
    while (!contacts.eof())
    {
        string s, m;
        contacts >> s;
        contacts >> m;
        trie->insertContact(s, m);
    }
    contacts.close();
    input.close();
    return true;
}
void WriteNewContact(Trie *trie)
{
    cout << "Enter the Name\nFormat: (FirstName_LastName : ";
    string NewContact;
    bool isfind;
    cin >> NewContact;
    transform(NewContact.begin(), NewContact.end(), NewContact.begin(), ::toupper);
    trie->findContact(NewContact, isfind);
    if (isfind)
    {
        cout << "\n\tContact already exists in the Phonebook" << endl;
        return;
    }
    cout << "Enter contact number : ";
    string number;
    cin >> number;
    //transform(number.begin(), number.end(), number.begin(), ::toupper);
    bool OnlyAlpha = true;
    for (int i = 0; i < NewContact.length(); i++)
    {
        if (!isalpha(NewContact[i]))
        {
            if (NewContact[i] == '_')
                continue;
            OnlyAlpha = false;
            break;
        }
    }
    if (OnlyAlpha)
    {
        ofstream out;
        out.open("contactlist.txt", ios::app);
        if (!out.is_open())
        {
            cout << "Sorry!\nCould not open the PhoneBook!\n";
            out.close();
            return;
        }
        else
        {
            out << NewContact << " " << number << "\n";
            cout << "\n\tSuccessfully loaded in the Phonebook!\n";
            out.close();
            trie->insertContact(NewContact, number);
            return;
        }
    }
    else
    {
        cout << "\nNot a valid contact name!\n";
        return;
    }
}

int main()
{
    Trie *trie = new Trie(); //Where new is used to allocate memory for a C++ class object, the object's constructor is called after the memory is allocated
    char mode;
    cout << "Loading the PhoneBook file" << endl;
    loadPhonebook(trie, "contactlist.txt");
    while (1)
    {
        cout << endl
             << endl;
        cout << "Interactive mode,press " << endl;
        cout << "1: Search" << endl;
        cout << "2: Enter new contacts into the phonebook\n";
        cout << "3: Remove contact" << endl;
        cout << "4: Quit" << endl
             << endl;
        cin >> mode;
        if (isalpha(mode))
        {
            cout << "Invalid Input!\n";
            cout << "Enter either 1 or 2..";
            continue;
        }
        switch (mode)
        {
        case '1':
        {
            string s;
            cout << "Enter the contact name to search in the Phonebook : ";
            cin >> s;
            transform(s.begin(), s.end(), s.begin(), ::toupper);
            trie->searchContact(s);
        }
            continue;
        case '2':
            WriteNewContact(trie);
            continue;
        case '3':
        {
            cout << "Enter the contact name you want to delete :";
            string contact;
            cin >> contact;
            transform(contact.begin(), contact.end(), contact.begin(), ::toupper);
            trie->remove(contact);
        }
            continue;
        case '4':
            delete trie;
            return 0;

        default:
            cout << "Invalid input!";
            cout << "\nEnter either 1 or 2..";
            continue;
        }
    }
}
