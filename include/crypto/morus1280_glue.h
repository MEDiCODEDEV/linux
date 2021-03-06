/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * The MORUS-1280 Authenticated-Encryption Algorithm
 *   Common glue skeleton -- header file
 *
 * Copyright (c) 2016-2018 Ondrej Mosnacek <omosnacek@gmail.com>
 * Copyright (C) 2017-2018 Red Hat, Inc. All rights reserved.
 */

#ifndef _CRYPTO_MORUS1280_GLUE_H
#define _CRYPTO_MORUS1280_GLUE_H

#include <linux/module.h>
#include <linux/types.h>
#include <crypto/algapi.h>
#include <crypto/aead.h>
#include <crypto/morus_common.h>

#define MORUS1280_WORD_SIZE 8
#define MORUS1280_BLOCK_SIZE (MORUS_BLOCK_WORDS * MORUS1280_WORD_SIZE)

struct morus1280_block {
	u8 bytes[MORUS1280_BLOCK_SIZE];
};

struct morus1280_glue_ops {
	void (*init)(void *state, const void *key, const void *iv);
	void (*ad)(void *state, const void *data, unsigned int length);
	void (*enc)(void *state, const void *src, void *dst, unsigned int length);
	void (*dec)(void *state, const void *src, void *dst, unsigned int length);
	void (*enc_tail)(void *state, const void *src, void *dst, unsigned int length);
	void (*dec_tail)(void *state, const void *src, void *dst, unsigned int length);
	void (*final)(void *state, void *tag_xor, u64 assoclen, u64 cryptlen);
};

struct morus1280_ctx {
	const struct morus1280_glue_ops *ops;
	struct morus1280_block key;
};

void crypto_morus1280_glue_init_ops(struct crypto_aead *aead,
				    const struct morus1280_glue_ops *ops);
int crypto_morus1280_glue_setkey(struct crypto_aead *aead, const u8 *key,
				 unsigned int keylen);
int crypto_morus1280_glue_setauthsize(struct crypto_aead *tfm,
				      unsigned int authsize);
int crypto_morus1280_glue_encrypt(struct aead_request *req);
int crypto_morus1280_glue_decrypt(struct aead_request *req);

#define MORUS1280_DECLARE_ALG(id, driver_name, priority) \
	static const struct morus1280_glue_ops crypto_morus1280_##id##_ops = {\
		.init = crypto_morus1280_##id##_init, \
		.ad = crypto_morus1280_##id##_ad, \
		.enc = crypto_morus1280_##id##_enc, \
		.enc_tail = crypto_morus1280_##id##_enc_tail, \
		.dec = crypto_morus1280_##id##_dec, \
		.dec_tail = crypto_morus1280_##id##_dec_tail, \
		.final = crypto_morus1280_##id##_final, \
	}; \
	\
	static int crypto_morus1280_##id##_init_tfm(struct crypto_aead *tfm) \
	{ \
		crypto_morus1280_glue_init_ops(tfm, &crypto_morus1280_##id##_ops); \
		return 0; \
	} \
	\
	static void crypto_morus1280_##id##_exit_tfm(struct crypto_aead *tfm) \
	{ \
	} \
	\
	static struct aead_alg crypto_morus1280_##id##_alg = { \
		.setkey = crypto_morus1280_glue_setkey, \
		.setauthsize = crypto_morus1280_glue_setauthsize, \
		.encrypt = crypto_morus1280_glue_encrypt, \
		.decrypt = crypto_morus1280_glue_decrypt, \
		.init = crypto_morus1280_##id##_init_tfm, \
		.exit = crypto_morus1280_##id##_exit_tfm, \
		\
		.ivsize = MORUS_NONCE_SIZE, \
		.maxauthsize = MORUS_MAX_AUTH_SIZE, \
		.chunksize = MORUS1280_BLOCK_SIZE, \
		\
		.base = { \
			.cra_flags = CRYPTO_ALG_INTERNAL, \
			.cra_blocksize = 1, \
			.cra_ctxsize = sizeof(struct morus1280_ctx), \
			.cra_alignmask = 0, \
			.cra_priority = priority, \
			\
			.cra_name = "__morus1280", \
			.cra_driver_name = "__"driver_name, \
			\
			.cra_module = THIS_MODULE, \
		} \
	}

#endif /* _CRYPTO_MORUS1280_GLUE_H */
