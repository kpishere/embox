/**
 * @file
 * @brief TODO documentation for slist.h -- Eldar Abusalimov
 *
 * @date 16.04.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_SLIST_IMPL_H_
#define UTIL_SLIST_IMPL_H_

#include <util/member.h>
#include <util/macro.h>

struct __slist_link {
	struct __slist_link *next;
};

#define __SLIST_LINK_INIT__(__link) \
	{                     \
		.next = (__link), \
	}

static inline void __slist_bind(struct __slist_link *prev,
		struct __slist_link *next) {
	prev->next = next;
}

static inline int __slist_link_alone(struct __slist_link *link) {
	return link == link->next;
}

static inline void __slist_link_init(struct __slist_link *link) {
	__slist_bind(link, link);
}

static inline void __slist_insert_chain(struct __slist_link *first,
		struct __slist_link *last, struct __slist_link *prev,
		struct __slist_link *next) {
	__slist_bind(prev, first);
	__slist_bind(last, next);
}

static inline void __slist_insert_link(struct __slist_link *link,
		struct __slist_link *prev, struct __slist_link *next) {
	__slist_insert_chain(link, link, prev, next);
}

#ifdef CONFIG_SLIST_NDEBUG
# include "slist_ndebug.h"
#else
# include "slist_debug.h"
#endif

/* Most of macros are defined through a corresponding _link method. */

#ifndef __slist_check
#define __slist_check(expr) (expr)
#endif

#define __slist_link_element(link, type, m_link) \
	member_out(link, type, m_link)

#define __slist_alone(element, m_link) \
	slist_alone_link(&__slist_check(element)->m_link)

/**
 * Casts the given @a link out to its container in case that it is not @c NULL.
 *
 * @param link
 *   An expression of type <em>struct slist_link *</em> being casted.
 * @param type
 *   The type of the container.
 * @param m_link
 *   The name of the member within the struct
 * @return
 *   The cast result if the @a link is not @c NULL.
 * @retval NULL
 *   If the @a link is @c NULL.
 */
#define __slist_link_safe_cast(link, type, m_link) \
	({ \
		struct slist_link *__slist_link__ = (link);                \
		__slist_link__                                             \
				? slist_link_element(__slist_link__, type, m_link) \
				: NULL;                                            \
	})

#define __slist_first(slist, type, m_link) \
	__slist_link_safe_cast(slist_first_link(slist), type, m_link)

#define __slist_add_first(element, slist, m_link) \
	slist_add_first_link(&__slist_check(element)->m_link, slist)

#define __slist_insert_after(element, slist_element, m_link) \
	slist_insert_after_link(&__slist_check(element)->m_link, \
			&__slist_check(slist_element)->m_link)

#define __slist_remove_first(slist, type, m_link) \
	__slist_link_safe_cast(slist_remove_first_link(slist), type, m_link)

#define __slist_foreach_cast_assign(_iter, element, member) \
	element = member_out(_iter, typeof(*element), member.l)

#define __slist_foreach(element, slist, m_link) \
	__slist_foreach__(element, slist, __slist_foreach_cast_assign, m_link)

#define __slist_foreach__(node, slist, iter_cast, cast_arg) \
	__slist_foreach_guarded(node, slist, iter_cast, cast_arg, \
			MACRO_GUARD(__iter), \
			MACRO_GUARD(__head), \
			MACRO_GUARD(__next))

#define __slist_foreach_guarded(node, slist, iter_cast_assign, cast_arg, \
		_iter, _head, _next) \
	for (struct __slist_link *_head = &__slist_check(slist)->l, \
				*_iter = (_head)->next, *_next = _iter->next; \
			_iter != _head && (iter_cast_assign(_iter, node, cast_arg)); \
			_iter = _next, _next = _iter->next)


#endif /* UTIL_SLIST_IMPL_H_ */
