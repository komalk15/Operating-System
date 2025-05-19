#include "../drivers/screen.h"
#include "../libc/string.h"
#include "../libc/mem.h"
#include "../memory/kheap.h"
#include "../cpu/timer.h"

char shell_name[15] = "SHELL";
extern char color_attribute;
enum vga_color color_vga;

// Simple filesystem structure
#define MAX_FILES 32
#define MAX_DIRS 16
#define MAX_NAME_LENGTH 32

typedef enum {
    FILE_TYPE_REGULAR,
    FILE_TYPE_DIRECTORY
} file_type_t;

typedef struct file {
    char name[MAX_NAME_LENGTH];
    file_type_t type;
    int parent_dir;  // Index of parent directory (-1 for root)
    int used;        // 1 if this entry is used, 0 if free
} file_t;

// Global filesystem
file_t filesystem[MAX_FILES];
int current_dir = 0; // Start at root directory (index 0)

// Initialize filesystem
void init_filesystem() {
    for (int i = 0; i < MAX_FILES; i++) {
        filesystem[i].used = 0;
    }
    
    // Create root directory
    // Replace strcpy with memory_copy
    memory_copy("/", filesystem[0].name, 2); // 2 bytes for "/" and null terminator
    filesystem[0].type = FILE_TYPE_DIRECTORY;
    filesystem[0].parent_dir = -1;
    filesystem[0].used = 1;
}

// Find a free slot in the filesystem
int find_free_slot() {
    for (int i = 1; i < MAX_FILES; i++) {
        if (!filesystem[i].used) {
            return i;
        }
    }
    return -1; // No free slots
}

// Check if a directory exists in the current directory
int find_dir(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used && 
            filesystem[i].type == FILE_TYPE_DIRECTORY &&
            filesystem[i].parent_dir == current_dir &&
            strcmp(filesystem[i].name, name) == 0) {
            return i;
        }
    }
    return -1; // Not found
}

// Helper function - check if file exists in current directory
int find_file(const char* name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used && 
            filesystem[i].type == FILE_TYPE_REGULAR &&
            filesystem[i].parent_dir == current_dir &&
            strcmp(filesystem[i].name, name) == 0) {
            return i;
        }
    }
    return -1; // Not found
}

void print_SAY_OS();

/* Commands declaration */
void command_help(char *input);
void command_name(char *input);
void command_color(char *input);
void command_picture(char *input);
void command_mkdir(char *input);
void command_ls(char *input);
void command_touch(char *input);
void command_cd(char *input);
void command_uptime(char *input);

void init_header()
{
    kprint_middle_row("SAY OS KERNEL SHELL (type help to get command name)\n\n", 0);
    fill_row_with_color(vga_entry_color(VGA_COLOR_WHITE, color_vga), 0);
}

void kshell()
{
    color_attribute = (char)vga_entry_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
    color_vga = VGA_COLOR_CYAN;
    init_header();
    print_SAY_OS();
    init_filesystem();
    kprint(shell_name);
    kprint("> ");
}

void user_input(char *input)
{
    char *command = strtok(input, ' ', 0);

    if (strcmp(input, "END") == 0)
    {
        kprint("Stopping the CPU. Bye!\n");
        asm volatile("hlt");
    }
    else if (strcmp(input, "CLEAR") == 0)
    {
        clear_screen();
        init_header();
    }
    else if (strcmp(command, "NAME") == 0)
    {
        command_name(input);
    }
    else if (strcmp(command, "HELP") == 0)
    {
        command_help(input);
    }
    else if (strcmp(command, "COLOR") == 0)
    {
        command_color(input);
    }
    else if (strcmp(command, "PICTURE") == 0)
    {
        command_picture(input);
    }
    else if (strcmp(command, "MKDIR") == 0)
    {
        command_mkdir(input);
    }
    else if (strcmp(command, "LS") == 0)
    {
        command_ls(input);
    }
    else if (strcmp(command, "TOUCH") == 0)
    {
        command_touch(input);
    }
    else if (strcmp(command, "CD") == 0)
    {
        command_cd(input);
    }
    else if (strcmp(command, "UPTIME") == 0)
    {
        command_uptime(input);
    }

    kfree(command);
    kprint(shell_name);
    kprint("> ");
}

void print_SAY_OS()
{
    kprint("\n");
    kprint("		  /$$$$$$   /$$$$$$  /$$     /$$        /$$$$$$   /$$$$$$ \n");
    kprint("		 /$$__  $$ /$$__  $$|  $$   /$$/       /$$__  $$ /$$__  $$\n");
    kprint("		| $$  \\__/| $$  \\ $$ \\  $$ /$$/       | $$  \\ $$| $$  \\__/\n");
    kprint("		|  $$$$$$ | $$$$$$$$  \\  $$$$/        | $$  | $$|  $$$$$$ \n");
    kprint("		 \\____  $$| $$__  $$   \\  $$/         | $$  | $$ \\____  $$\n");
    kprint(" 		/$$  \\ $$| $$  | $$    | $$          | $$  | $$ /$$  \\ $$\n");
    kprint("		|  $$$$$$/| $$  | $$    | $$          |  $$$$$$/|  $$$$$$/\n");
    kprint(" 		\\______/ |__/  |__/    |__/           \\______/  \\______/\n");
    kprint("\n");
}

void command_help(char *input)
{
    char *help = strtok(input, ' ', 1);

    if (!help)
        kprint("\tList of commands\n\tNAME END CLEAR COLOR HELP PICTURE MKDIR LS TOUCH CD UPTIME\n\tType `HELP NAME` to find more about command NAME.\n");
    else if (strcmp(help, "NAME") == 0)
    {
        kprint("\tNAME : Give 'name' argument to change shell name.\n");
    }
    else if (strcmp(help, "COLOR") == 0)
    {
        kprint("\tCOLOR : Give 'color' argument to change shell color.\n");
        kprint("\tOptions : Red, Green, Blue, Cyan, White\n");
    }
    else if (strcmp(help, "CLEAR") == 0)
    {
        kprint("\tCLEAR : Clears the screen.\n");
    }
    else if (strcmp(help, "END") == 0)
    {
        kprint("\tEND : Halts the CPU.\n");
    }
    else if (strcmp(help, "PICTURE") == 0)
    {
        kprint("\tPICTURE : Prints a picture\n");
        kprint("\tAvailable picture: RAIN\n");
    }
    else if (strcmp(help, "MKDIR") == 0)
    {
        kprint("\tMKDIR : Creates a new directory\n");
        kprint("\tUsage: MKDIR <directory_name>\n");
    }
    else if (strcmp(help, "LS") == 0)
    {
        kprint("\tLS : Lists directories and files in the current directory\n");
    }
    else if (strcmp(help, "TOUCH") == 0)
    {
        kprint("\tTOUCH : Creates a new empty file\n");
        kprint("\tUsage: TOUCH <filename>\n");
    }
    else if (strcmp(help, "CD") == 0)
    {
        kprint("\tCD : Change current directory\n");
        kprint("\tUsage: CD <directory_name> or CD .. to go up one level\n");
    }
    else if (strcmp(help, "UPTIME") == 0)
    {
        kprint("\tUPTIME : Display how long the system has been running\n");
    }

    kfree(help);
}

void command_name(char *input)
{
    char *name = strtok(input, ' ', 1);

    if (!name)
        kprint("1 Argument Required!\n");
    else
    {
        memory_copy(name, shell_name, strlen(name));
        shell_name[strlen(name)] = '\0';
    }
    kfree(name);
}

void command_color(char *input)
{
    char *color = strtok(input, ' ', 1);

    if (!color)
        kprint("1 Argument Required!\n");
    else if (strcmp(color, "RED") == 0)
    {
        color_attribute = (char)vga_entry_color(VGA_COLOR_RED, VGA_COLOR_BLACK);
        color_vga = VGA_COLOR_RED;
        change_text_color((uint8_t)color_attribute);
        fill_row_with_color(vga_entry_color(VGA_COLOR_WHITE, color_vga), 0);
    }
    else if (strcmp(color, "GREEN") == 0)
    {
        color_attribute = (char)vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK);
        color_vga = VGA_COLOR_GREEN;
        change_text_color((uint8_t)color_attribute);
        fill_row_with_color(vga_entry_color(VGA_COLOR_WHITE, color_vga), 0);
    }
    else if (strcmp(color, "BLUE") == 0)
    {
        color_attribute = (char)vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK);
        color_vga = VGA_COLOR_BLUE;
        change_text_color((uint8_t)color_attribute);
        fill_row_with_color(vga_entry_color(VGA_COLOR_WHITE, color_vga), 0);
    }
    else if (strcmp(color, "CYAN") == 0)
    {
        color_attribute = (char)vga_entry_color(VGA_COLOR_CYAN, VGA_COLOR_BLACK);
        color_vga = VGA_COLOR_CYAN;
        change_text_color((uint8_t)color_attribute);
        fill_row_with_color(vga_entry_color(VGA_COLOR_WHITE, color_vga), 0);
    }
    else if (strcmp(color, "WHITE") == 0)
    {
        color_attribute = (char)vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        color_vga = VGA_COLOR_WHITE;
        change_text_color((uint8_t)color_attribute);
        fill_row_with_color(vga_entry_color(VGA_COLOR_WHITE, color_vga), 0);
    }
    kfree(color);
}

void command_picture(char *input)
{
    char *picture = strtok(input, ' ', 1);
    if (!picture)
    {
        print_SAY_OS();
    }
    else if (strcmp(picture, "RAIN") == 0)
    {
        kprint("      __   _\n");
        kprint("    _(  )_( )_\n");
        kprint("   (_   _    _)\n");
        kprint("  / /(_) (__)\n");
        kprint(" / / / / / /\n");
        kprint("/ / / / / /\n");
    }

    kfree(picture);
}

void command_mkdir(char *input)
{
    char *dirname = strtok(input, ' ', 1);

    if (!dirname) {
        kprint("1 Argument Required! Usage: MKDIR <directory_name>\n");
    } else {
        // Check if name is valid (not too long)
        if (strlen(dirname) >= MAX_NAME_LENGTH) {
            kprint("Error: Directory name too long (max 31 characters)\n");
        }
        // Check if directory already exists
        else if (find_dir(dirname) != -1) {
            kprint("Error: Directory already exists\n");
        } else {
            // Find a free slot
            int slot = find_free_slot();
            if (slot == -1) {
                kprint("Error: Filesystem is full\n");
            } else {
                // Create directory - use memory_copy instead of strcpy
                memory_copy(dirname, filesystem[slot].name, strlen(dirname) + 1);
                filesystem[slot].type = FILE_TYPE_DIRECTORY;
                filesystem[slot].parent_dir = current_dir;
                filesystem[slot].used = 1;
                
                kprint("Directory created: ");
                kprint(dirname);
                kprint("\n");
            }
        }
    }
    
    kfree(dirname);
}

void command_ls(char *input)
{
    int count = 0;
    
    // Print current directory name
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used && i == current_dir) {
            kprint("Current directory: ");
            kprint(filesystem[i].name);
            kprint("\n");
            break;
        }
    }
    
    kprint("Contents:\n");
    
    // List all directories in the current directory
    for (int i = 0; i < MAX_FILES; i++) {
        if (filesystem[i].used && 
            filesystem[i].parent_dir == current_dir && 
            i != current_dir) {
            
            kprint("\t");
            if (filesystem[i].type == FILE_TYPE_DIRECTORY) {
                kprint("[DIR] ");
            } else {
                kprint("[FILE] ");
            }
            kprint(filesystem[i].name);
            kprint("\n");
            count++;
        }
    }
    
    if (count == 0) {
        kprint("\t(empty)\n");
    }
}

void command_touch(char *input)
{
    char *filename = strtok(input, ' ', 1);

    if (!filename) {
        kprint("1 Argument Required! Usage: TOUCH <filename>\n");
    } else {
        // Check if name is valid (not too long)
        if (strlen(filename) >= MAX_NAME_LENGTH) {
            kprint("Error: Filename too long (max 31 characters)\n");
        }
        // Check if file already exists
        else if (find_file(filename) != -1) {
            kprint("Error: File already exists\n");
        } 
        // Check if directory with same name exists
        else if (find_dir(filename) != -1) {
            kprint("Error: A directory with this name already exists\n");
        }
        else {
            // Find a free slot
            int slot = find_free_slot();
            if (slot == -1) {
                kprint("Error: Filesystem is full\n");
            } else {
                // Create file
                memory_copy(filename, filesystem[slot].name, strlen(filename) + 1);
                filesystem[slot].type = FILE_TYPE_REGULAR;
                filesystem[slot].parent_dir = current_dir;
                filesystem[slot].used = 1;
                
                kprint("File created: ");
                kprint(filename);
                kprint("\n");
            }
        }
    }
    
    kfree(filename);
}

void command_cd(char *input)
{
    char *dirname = strtok(input, ' ', 1);
    
    if (!dirname) {
        kprint("1 Argument Required! Usage: CD <directory_name> or CD ..\n");
    } else if (strcmp(dirname, "..") == 0) {
        // Go up one directory level (to parent directory)
        if (current_dir != 0) { // Not at root
            // Get parent directory index
            current_dir = filesystem[current_dir].parent_dir;
            kprint("Changed to parent directory\n");
        } else {
            kprint("Already at root directory\n");
        }
    } else if (strcmp(dirname, "/") == 0) {
        // Go to root directory
        current_dir = 0;
        kprint("Changed to root directory\n");
    } else {
        // Find the directory in current directory
        int dir_index = find_dir(dirname);
        
        if (dir_index == -1) {
            kprint("Error: Directory not found: ");
            kprint(dirname);
            kprint("\n");
        } else {
            // Change to the directory
            current_dir = dir_index;
            kprint("Changed to directory: ");
            kprint(dirname);
            kprint("\n");
        }
    }
    
    kfree(dirname);
}

// Helper function to convert an integer to a string
void int_to_str(int num, char* str, int base) {
    int i = 0;
    int isNegative = 0;
    
    // Handle 0 explicitly
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    // Handle negative numbers (for base 10 only)
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }
    
    // Process individual digits
    while (num != 0) {
        int remainder = num % base;
        str[i++] = (remainder < 10) ? remainder + '0' : remainder - 10 + 'a';
        num = num / base;
    }
    
    // Add negative sign if needed
    if (isNegative) {
        str[i++] = '-';
    }
    
    // Null terminate the string
    str[i] = '\0';
    
    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void command_uptime(char *input)
{
    // Get the current tick count from the timer
    extern uint32_t tick;
    
    // Calculate time units
    uint32_t seconds = tick / 100; // Assuming 100Hz timer
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    uint32_t days = hours / 24;
    
    // Adjust values to show remainders
    seconds %= 60;
    minutes %= 60;
    hours %= 24;
    
    // Prepare strings for output
    char days_str[20];
    char hours_str[20];
    char minutes_str[20];
    char seconds_str[20];
    
    int_to_str(days, days_str, 10);
    int_to_str(hours, hours_str, 10);
    int_to_str(minutes, minutes_str, 10);
    int_to_str(seconds, seconds_str, 10);
    
    kprint("System uptime: ");
    
    if (days > 0) {
        kprint(days_str);
        kprint(" day(s), ");
    }
    
    kprint(hours_str);
    kprint(" hour(s), ");
    kprint(minutes_str);
    kprint(" minute(s), ");
    kprint(seconds_str);
    kprint(" second(s)\n");
}