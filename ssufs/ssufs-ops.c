#include "ssufs-ops.h"

extern struct filehandle_t file_handle_array[MAX_OPEN_FILES];

int ssufs_allocFileHandle() {
	for(int i = 0; i < MAX_OPEN_FILES; i++) {
		if (file_handle_array[i].inode_number == -1) {
			return i;
		}
	}
	return -1;
}

int ssufs_create(char *filename){
	/* 1 */
    
    int inodenum = open_namei(filename);
    if(inodenum != -1) return -1;
    else {
        inodenum = ssufs_allocInode();
        if(inodenum == -1) return -1;
        else {
            struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
            ssufs_readInode(inodenum, inode);
            inode->status = INODE_IN_USE;
            memcpy(inode->name, filename, MAX_NAME_STRLEN);
            inode->file_size = 0;
            for(int i = 0; i < MAX_FILE_SIZE; i++)
                inode->direct_blocks[i] = -1;
            ssufs_writeInode(inodenum, inode);
            free(inode);    
            return inodenum;
        }
    }

    return -1;
}

void ssufs_delete(char *filename){
	/* 2 */

    int inodenum = open_namei(filename);
    struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
    memcpy(inode->name, "", 1);
    for(int i = 0; i < MAX_FILE_SIZE; i++) 
        if(inode->direct_blocks[i] == DATA_BLOCK_USED)
            ssufs_freeDataBlock(inode->direct_blocks[i]);
    ssufs_writeInode(inodenum, inode);
    ssufs_freeInode(inodenum);
    for(int i = 0; i < MAX_OPEN_FILES; i++)
        if(file_handle_array[i].inode_number == inodenum) ssufs_close(i);
}

int ssufs_open(char *filename){
	/* 3 */

    int inodenum = open_namei(filename);
    if(inodenum == -1) return -1;
    else {
        int handlenum = ssufs_allocFileHandle();
        if(handlenum == -1) return -1;
        else {
            file_handle_array[handlenum].inode_number = inodenum;
            file_handle_array[handlenum].offset = 0;

            return handlenum;
        }
    }
    return -1;
}

void ssufs_close(int file_handle){
	file_handle_array[file_handle].inode_number = -1;
	file_handle_array[file_handle].offset = 0;
}

int ssufs_read(int file_handle, char *buf, int nbytes){
	/* 4 */

	int offset = file_handle_array[file_handle].offset;

	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(file_handle_array[file_handle].inode_number, tmp);

	int fsize = tmp->file_size;

	if ((fsize == -1) || (offset < 0) || (offset > fsize)) {
		free(tmp);
		return -1;
	}

    char *str = (char *)malloc(sizeof(char) * BLOCKSIZE * 4);
    char *temp = (char *)malloc(sizeof(char) * BLOCKSIZE);
    
    int len = 0;
    
    for(int i = 0; i <= 4; i++) {
        if(tmp->direct_blocks[i] != -1) {
            ssufs_readDataBlock(tmp->direct_blocks[i], temp);
            strcat(str + len, temp);
            len += strlen(temp);
        }
    }

    memcpy(buf, str + offset, nbytes);

    file_handle_array[file_handle].offset += nbytes;

    return 0;
}

int ssufs_write(int file_handle, char *buf, int nbytes){
	/* 5 */
	int offset = file_handle_array[file_handle].offset;

	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(file_handle_array[file_handle].inode_number, tmp);

	int fsize = tmp->file_size;

	if ((offset < 0) || (offset + nbytes > BLOCKSIZE * 4)) {
		free(tmp);
		return -1;
	}

    int from = offset / BLOCKSIZE;
    int to = (offset + nbytes -1) / BLOCKSIZE;

    int need_block = ((offset + nbytes -1) / BLOCKSIZE) - (offset / BLOCKSIZE) +1;
    int check = 0, offset_t = 0;
    int backup = 0;
    char *temp = (char *)malloc(BLOCKSIZE);
    char *temp2 = (char *)malloc(BLOCKSIZE);
    memset(temp, 0, BLOCKSIZE);
    memset(temp2, 0, BLOCKSIZE);

    for(int i = from; i <= to; i++) {
        if(offset % 64 == 0 || check == 1) {
            int blocknum = ssufs_allocDataBlock();
            memcpy(temp, buf + offset_t, BLOCKSIZE);
            ssufs_writeDataBlock(blocknum, temp);
            offset_t += BLOCKSIZE;
            tmp->direct_blocks[i] = blocknum;
            memset(temp, 0, BLOCKSIZE);
        }
        else {
            ssufs_readDataBlock(tmp->direct_blocks[i], temp);
            backup = i;
            memcpy(temp2, temp, BLOCKSIZE);
            strncat(temp2, buf, BLOCKSIZE - strlen(temp));
            ssufs_writeDataBlock(tmp->direct_blocks[i], temp2);
            offset_t += BLOCKSIZE - strlen(temp);
            memset(temp2, 0, BLOCKSIZE);
            check = 1;
        }
    }

    offset += nbytes;
    tmp->file_size += nbytes;

    if(tmp->file_size > BLOCKSIZE * 4) {
        for(int i = from; i <= to; i++)
            ssufs_freeDataBlock(i);
        ssufs_writeDataBlock(backup, temp);
    }
    
    file_handle_array[file_handle].offset = offset;

    ssufs_writeInode(file_handle_array[file_handle].inode_number, tmp);
    

    return 0;

}

int ssufs_lseek(int file_handle, int nseek){
	int offset = file_handle_array[file_handle].offset;

	struct inode_t *tmp = (struct inode_t *) malloc(sizeof(struct inode_t));
	ssufs_readInode(file_handle_array[file_handle].inode_number, tmp);
	
	int fsize = tmp->file_size;
	
	offset += nseek;

	if ((fsize == -1) || (offset < 0) || (offset > fsize)) {
		free(tmp);
		return -1;
	}

	file_handle_array[file_handle].offset = offset;
	free(tmp);

	return 0;
}
