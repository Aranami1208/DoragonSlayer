#pragma once
//�}�N����`
#define SAFE_RELEASE(x) if(x){x->Release(); x=nullptr;}
#define SAFE_DELETE(x) if(x){delete x; x=nullptr;}
#define SAFE_DELETE_ARRAY(p){ if(p){ delete[] (p);   (p)=nullptr;}}
