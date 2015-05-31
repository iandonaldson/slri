#define SHOUND_TI_SEQENTRY 2
#define SHOUND_TI_BIOSEQ 1


Boolean SHound_TI_ExpandSeqEntryRecList(ValNodePtr pvnrecs)
{
   ValNodePtr pvnlast = NULL, pvntemp = NULL;

   if (pvnrecs == NULL)
   {
     ErrPostEx(SEV_ERROR,0,0,"SHound_TI_ExpandSeqEntryRecList: Invalid parameter.");
     return FALSE;
   }

   while(pvnrecs != NULL)
   {
     if(pvnrecs->choice == SHOUND_TI_SEQENTRY)
     {
        pvngis = SHoundGetGIsFromSeqEntry(pvnrecs->data.intvalue);
        if(pvngis == NULL)
        {
           ErrPostEx(SEV_ERROR,0,0,"SHound_TI_ExpandSeqEntryRecList: List of SeqEntry GIs for %ld not found.", (long) pvnrecs->data.intvalue);
           return FALSE;
        }
        /* remove current node, it will get replaced by expansion of SeqEntry */
        pvntemp = pvnrecs;
        pvnrecs = pvnlast;
        if(pvntemp->next != NULL)
          pvnrecs->next = pvntemp->next;
        else
          pvnrecs->next = NULL;
        pvntemp = MemFree(pvntemp);
        /* insert the list of GIs from SeqEntry */
        pvnrecs = Misc_ValNodeInsert(&pvnrecs, pvngis);
     }

     pnvlast = pvnrecs;
     pvnrecs = pvnrecs->next;
   }
}

/* inserts a ValNode list into a ValNode list behind the position of the input poiter
returns poiter value to last element of the insertion list */
ValNodePtr Misc_ValNodeInsert(ValNodePtr PNTR ppvnlist, ValNodePtr pvnins)
{
   ValNodePtr pvnlist = NULL, pvntemp = NULL;

   if(*ppvnlist == NULL)
   {
     /* SK: NULL ptr check */
     while(pvnins != NULL && pvnins->next != NULL)
       pvnins = pvnins->next;
     return pvnins;
   }

   if(pvnins == NULL)
     return *ppvnlist;

   pvnlist = *ppvnlist;
   /* SK: NULL ptr check */
   if(pvnlist != NULL && pvnlist->next != NULL)
      pvntemp = pvnlist->next;
   pvnlist->next = pvnins;
   /* SK: NULL ptr check */
   while(pvnins != NULL && pvnins->next != NULL)
     pvnins = pvnins->next;
   pvnins->next = pvntemp;
   return pvnins;
}


ValNodePtr SHoundGetGIsFromSeqEntry(Int4 gi)
{
   StSengiPtr psg = NULL, psg2 = NULL;
   ValNodePtr pvngi = NULL;


   psg = StSengiNew();
   psg->gi = gi;
   if ((res = SearchSENGI(&psg)) == -1)
   {
      StSengiFree(psg);
      return NULL;
   }
   else if (res == 0)
   {
      StSengiFree(psg);
      return NULL;
   }
   else
   {
      psg2 = StSengiNew();
      psg2->seid = psg->seid;
      if ((res = SearchSENGI(&psg2)) == -1)
      {
        StSengiFree(psg);
        StSengiFree(psg2);
        return NULL;
      }
      else if (res == 0)
      {
        StSengiFree(psg);
        StSengiFree(psg2);
        return NULL;
      }
      else
      {
        while(psg2 != NULL)
        {
          pvngi = Slri_ValNodeAddInt(&pvngi, 0, psg2->gi);
          psg2 = psg2->next;
        }
        StSengiFree(psg);
        StSengiFree(psg2);
        return pvngi;
      }
   }
   return NULL;
}
