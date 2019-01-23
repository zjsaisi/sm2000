/* for Hitachi H8/300H and H8S */
extern char *_D_BGN, *_D_END, *_D_ROM, *_B_BGN, *_B_END;
void cstart()
{
	char *src_ptr, *dest_ptr;

	for (dest_ptr = _B_BGN; dest_ptr < _B_END; dest_ptr++)
		*dest_ptr = 0;

	for (dest_ptr = _D_BGN, src_ptr = _D_ROM; dest_ptr < _D_END; dest_ptr++, src_ptr++)
		*dest_ptr = *src_ptr;
}
