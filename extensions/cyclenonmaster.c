void
cyclenonmaster(const Arg *arg)
{
	Client *lm = NULL, *fc = NULL, *lc = NULL;
	Client *plc = NULL, *pfc = NULL, *nfc = NULL;

	// Find last master client
	if (selmon->nmaster > 0) {
		{
			Client *c = selmon->clients;
			lm = c;
			int nm = selmon->nmaster;
			while (c && nm > 0) {
				if (ISVISIBLE(c) && !c->isfloating) {
					--nm;
					lm = c;
				}
				c = c->next;
			}
			if (!lm)
				return;
		}
	} else return;

	// Find first nonmaster client
	fc = lm ? lm->next : selmon->clients;
	for (; fc && (!ISVISIBLE(fc) || fc->isfloating); fc = fc->next);
	if (!fc)
		return;

	// Find last nonmaster client
	{
		Client *c = fc;
		while (c) {
			if (ISVISIBLE(c) && !c->isfloating)
				lc = c;
			c = c->next;
		}
	}

	if (fc == lc)
		return;

	// Find client before last nonmaster client
	for (plc = fc; plc->next != lc; plc = plc->next);

	// Find client before first nonmaster client
	if (!(selmon->clients == fc))
		for (pfc = selmon->clients; pfc->next != fc; pfc = pfc->next);

	if (arg->i > 0) {
		plc->next = lc->next;
		pfc->next = lc;
		lc->next = fc;
	} else {
		pfc->next = fc->next;;
		fc->next = lc->next;
		lc->next = fc;
	}

	// Perform arrangement changes
	{
		Client *c = selmon->sel;
		arrange(selmon);
		focus(pfc->next);
		focus(c);
		restack(selmon);
	}
}

