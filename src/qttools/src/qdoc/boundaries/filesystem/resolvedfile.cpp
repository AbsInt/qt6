// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0

#include "resolvedfile.h"

/*!
 * \class ResolvedFile
 *
 * \brief Represents a file that is reachable by QDoc based on its
 * current configuration.
 *
 * Instances of this type are, generally, intended to be generated by
 * any process that needs to query the filesystem for the presence of
 * some files based on a user-inputted path to ensure their
 * availability.
 *
 * Such an example might be when QDoc is searching for a file whose
 * path is provided by the user, such as the one in a snippet command,
 * that should represent a file that is reachable with the current
 * configuration.
 *
 * On the other side, logic that requires access to files that are
 * known to be user-provided, such as the quoting of snippets, can use
 * this type at the API boundary to signal that the file should be
 * accessible so that they avoid the need to search for the file
 * themselves.
 *
 * Do note that, semantically, this type doesn't actually guarantee
 * anything about its origin and only guarantees whatever its members
 * guarantee.
 *
 * The reasoning behind this lack of enforcement is to allow for an
 * easier testing.
 * As many parts of QDoc might require the presence of an instance of
 * this type, we want to be able to construct those instances without
 * the need to pass trough whichever valid generator for them.
 *
 * Nonetheless, inside QDoc, any boundary that requires an instance of
 * this type can consider it guaranteed that the instance was
 * generated trough some appropriate logic, and consider it a bug if
 * such is not the case.
 *
 * An instance of this type provides two pieces of information.
 *
 * The path to the file that is considered resolved, accessible trough
 * the get_path() method and the string that was used to resolve the
 * file in the first place, accessible trough the get_query() method.
 *
 * The first should be used by consumer who needs to interact with the
 * file itself, such as reading from it or copying it.
 *
 * The second is provided for context and can be used when consumers
 * need to know what the user-inputted path was in the first place,
 * for example when presenting debug information.
 *
 * It is not semantically guaranteed that this two pieces of
 * information are actually related. Any such instance for which this
 * is true should be considered malformed. Inside QDoc, tough,
 * consumer of this type can consider it guaranteed that no malformed
 * instance will be passed to them, and consider it a bug if it
 * happens otherwise.
 */

/*!
 * \fn  ResolvedFile::ResolvedFile(QString query, FilePath filepath)
 *
 * Constructs an instance of this type from \a query and \a filepath.
 *
 * \a query should represent the user-inputted path that was used to
 * resolve the file that this instance represents.
 *
 * \a filepath should represent the file that is found querying the
 * filesystem trough \a query using an appropriate logic for resolving
 * files.
 *
 * An instance that is built from \a query and \a filepath is
 * guaranteed to return a value that is equivalent to \a query when
 * get_query() is called and a value that is equivalent to \a
 * filepath.value() when get_path() is called.
 */

/*!
 * \fn const QString& ResolvedFile::get_query() const
 *
 * Returns a string representing the user-inputted path that was used
 * to resolve the file.
 */

/*!
 * \fn const QString& ResolvedFile::get_path() const
 *
 * Returns a string representing the canonicalized path to the file
 * that was resolved.
 *
 * Access to this file is to be considered guranteed to be available.
 */
