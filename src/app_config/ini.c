#include "ini.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"

static ini_runtime_t s_runtime;
static char s_current_section[CONFIG_SECTION_LENGTH];

static bool ini_add_entry(const char *section,const char *key,const char *value)
{
    if (!section || !key || !value) return false;
    if (s_runtime.count >= CONFIG_MAX_ENTRIES) return false;

    config_entry_t *e = &s_runtime.entries[s_runtime.count++];

    strncpy(e->section, section, CONFIG_SECTION_LENGTH-1);
    e->section[CONFIG_SECTION_LENGTH-1]='\0';

    strncpy(e->key, key, CONFIG_KEY_LENGTH-1);
    e->key[CONFIG_KEY_LENGTH-1]='\0';

    strncpy(e->value, value, CONFIG_VALUE_LENGTH-1);
    e->value[CONFIG_VALUE_LENGTH-1]='\0';

    return true;
}

static bool ini_parse_line(char *line)
{
    line[strcspn(line,"\r\n")] = '\0';

    if(line[0]=='\0' || line[0]=='#' || line[0]==';')
        return true;

    if(line[0]=='[')
    {
        char *end = strchr(line,']');
        if(!end) return false;

        *end='\0';

        strncpy(s_current_section,line+1,sizeof(s_current_section)-1);
        s_current_section[sizeof(s_current_section)-1]='\0';
        return true;
    }

    char *eq = strchr(line,'=');
    if(!eq) return false;
    if(s_current_section[0]=='\0') return false;

    *eq='\0';

    return ini_add_entry(s_current_section,line,eq+1);
}

bool ini_init(const char *filename)
{
    if(!filename) return false;

    memset(&s_runtime,0,sizeof(s_runtime));
    s_current_section[0]='\0';

    FILE *fp=fopen(filename,"r");
    if(!fp)
    {
        LOG_ERROR("INI","Unable to open %s",filename);
        return false;
    }

    char line[256];
    while(fgets(line,sizeof(line),fp))
    {
        if(!ini_parse_line(line))
        {
            fclose(fp);
            LOG_ERROR("INI","Parse error");
            return false;
        }
    }

    fclose(fp);
    s_runtime.initialized=true;
    return true;
}

bool ini_get_string(const char *section,const char *key,char *buffer,size_t n)
{
    if(!section||!key||!buffer||n==0) return false;

    for(size_t i=0;i<s_runtime.count;i++)
    {
        const config_entry_t *e=&s_runtime.entries[i];
        if(strcmp(e->section,section)==0 && strcmp(e->key,key)==0)
        {
            strncpy(buffer,e->value,n-1);
            buffer[n-1]='\0';
            return true;
        }
    }
    return false;
}

bool ini_get_int(const char *s,const char *k,int *v)
{
    char b[32];
    if(!v || !ini_get_string(s,k,b,sizeof(b))) return false;
    *v=(int)strtol(b,NULL,10);
    return true;
}

bool ini_get_float(const char *s,const char *k,float *v)
{
    char b[32];
    if(!v || !ini_get_string(s,k,b,sizeof(b))) return false;
    *v=strtof(b,NULL);
    return true;
}

bool ini_get_bool(const char *s,const char *k,bool *v)
{
    char b[16];
    if(!v || !ini_get_string(s,k,b,sizeof(b))) return false;

    if(!strcmp(b,"true")||!strcmp(b,"TRUE")||!strcmp(b,"1")||!strcmp(b,"yes"))
    { *v=true; return true; }

    if(!strcmp(b,"false")||!strcmp(b,"FALSE")||!strcmp(b,"0")||!strcmp(b,"no"))
    { *v=false; return true; }

    return false;
}

const ini_runtime_t *ini_get_runtime(void)
{
    return &s_runtime;
}
