#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preprocessor.h"
#include "table.h"
#include "parser.h"
#include "codegen.h"
#include <dirent.h>
#include <sys/stat.h>

static int has_vm_ext(const char *name)
{
    const char *dot = strrchr(name, '.');
    return dot && strcmp(dot, ".vm") == 0;
}

static char *strip_ext(const char *name)
{
    char *c = strdup(name);
    char *dot = strrchr(c, '.');
    if (dot)
        *dot = '\0';
    return c;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <file.vm|directory>\n", argv[0]);
        return 1;
    }
    const char *inpath = argv[1];

    struct stat st;
    if (stat(inpath, &st) != 0)
    {
        perror("stat");
        return 2;
    }

    table *t = initTable();
    buildVMTable(t);

    if (S_ISDIR(st.st_mode))
    {
        // directory mode: collect .vm files, sort, and concatenate outputs
        DIR *d = opendir(inpath);
        if (!d)
        {
            perror("opendir");
            return 3;
        }
        char **names = NULL;
        size_t n = 0;
        struct dirent *ent;
        while ((ent = readdir(d)) != NULL)
        {
            if (ent->d_type == DT_REG || ent->d_type == DT_UNKNOWN)
            {
                if (has_vm_ext(ent->d_name))
                {
                    names = realloc(names, (n + 1) * sizeof(char *));
                    names[n++] = strdup(ent->d_name);
                }
            }
        }
        closedir(d);
        if (n == 0)
        {
            free(names);
            return 0;
        }
        // sort names
        qsort(names, n, sizeof(char *), (int (*)(const void *, const void *))strcmp);

        // build output path: <dir>/<dirbasename>.asm
        const char *base = strrchr(inpath, '/');
        base = base ? base + 1 : inpath;
        char outpath[1024];
        snprintf(outpath, sizeof(outpath), "%s/%s.asm", inpath, base);
        FILE *out = fopen(outpath, "w");
        if (!out)
        {
            perror("fopen out");
            for (size_t i = 0; i < n; i++)
                free(names[i]);
            free(names);
            return 4;
        }

        // process files in sorted order
        for (size_t i = 0; i < n; i++)
        {
            char filepath[1024];
            snprintf(filepath, sizeof(filepath), "%s/%s", inpath, names[i]);
            FILE *in = fopen(filepath, "r");
            if (!in)
            {
                perror("fopen");
                continue;
            }
            char *basename = strip_ext(names[i]);
            char *line = NULL;
            size_t len = 0;
            while (getline(&line, &len, in) != -1)
            {
                stripComment(line);
                stripWhiteSpace(line);
                if (line[0] == '\0')
                    continue;
                VMPacket *pkt = buildVMPacket(line, t);
                char *asmstr = codeGen(pkt, basename);
                if (asmstr)
                {
                    fputs(asmstr, out);
                    free(asmstr);
                }
            }
            free(line);
            free(basename);
            fclose(in);
        }

        for (size_t i = 0; i < n; i++)
            free(names[i]);
        free(names);
        fclose(out);
        return 0;
    }
    else
    {
        // single file mode
        FILE *in = fopen(inpath, "r");
        if (!in)
        {
            perror("fopen");
            return 2;
        }

        // build output path by replacing .vm with .asm
        char outpath[1024];
        strncpy(outpath, inpath, sizeof(outpath) - 1);
        outpath[sizeof(outpath) - 1] = '\0';
        char *dot = strrchr(outpath, '.');
        if (dot)
            strcpy(dot, ".asm");
        else
            strncat(outpath, ".asm", sizeof(outpath) - strlen(outpath) - 1);

        // derive base filename for static symbol namespace
        const char *base = strrchr(inpath, '/');
        if (base)
            base++;
        else
            base = inpath;
        char basename[256];
        strncpy(basename, base, sizeof(basename) - 1);
        basename[sizeof(basename) - 1] = '\0';
        char *ext = strrchr(basename, '.');
        if (ext)
            *ext = '\0';

        char *line = NULL;
        size_t len = 0;
        FILE *out = fopen(outpath, "w");
        if (!out)
        {
            perror("fopen out");
            fclose(in);
            return 3;
        }

        while (getline(&line, &len, in) != -1)
        {
            stripComment(line);
            stripWhiteSpace(line);
            if (line[0] == '\0')
                continue;
            VMPacket *pkt = buildVMPacket(line, t);
            char *asmstr = codeGen(pkt, basename);
            if (asmstr)
            {
                fputs(asmstr, out);
                free(asmstr);
            }
        }

        free(line);
        fclose(in);
        fclose(out);
        return 0;
    }
}
