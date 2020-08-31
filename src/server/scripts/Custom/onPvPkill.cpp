// Made By Sinistah/ToxicDev

enum config
{
    TOKEN = 11111, // Entry Id Of The Item Defined Here
    AMMOUNT = 1,    // Ammount Of The TOKEN's Given
    AREAIDONE = 172, // Area Id For Location One
    AREAIDTWO = 232 // Area Id For Location Two
};  

class itemonkill : public PlayerScript
{
public: 
    itemonkill() : PlayerScript("itemonkill") { }

    void OnPVPKill(Player * killer, Player * victim)
    {
        if(killer->GetGUID() == victim->GetGUID())
            killer->GetSession()->SendNotification("[PvP System] Suicide!");
        else if (killer->GetSession()->GetRemoteAddress() == victim->GetSession()->GetRemoteAddress())
            killer->GetSession()->SendNotification("[PvP System] Your Victim Has Same Ip Adress As You! No Reward Was Given.");
        else if (victim->HasAura(2479))
            killer->GetSession()->SendNotification("[PvP System] Your Victim Had Honorless Target Aura! No Reward Was Given");
        else if (killer->GetAreaId() == AREAIDONE || killer->GetAreaId() == AREAIDTWO)
        {
            killer->GetSession()->SendNotification("[PvP System] You Were Rewarded For Killing %s!", victim->GetName());    
            killer->AddItem(TOKEN, AMMOUNT);
        }
    }
};

void AddSC_itemonkill()
{
    new itemonkill;
}
