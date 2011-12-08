/**  This file is part of SpotifyResolver for Tomahawk Player - <hugolm84@gmail.com>
 *
 *   Copyright 2011,Hugo Lindström <hugolm84@gmail.com>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 */

#ifndef SPOTIFYPlaylist_H
#define SPOTIFYPlaylist_H


#include <QObject>
#include <QVariant>
#include <libspotify/api.h>


class SpotifyPlaylist : public QObject
{
    Q_OBJECT
public:
    explicit SpotifyPlaylist(sp_session *session, QObject* parent = 0 );
    static void SP_CALLCONV pc_loaded( sp_playlistcontainer *pc, void *userdata );
    static void SP_CALLCONV pc_playlist_added( sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata );
    static void SP_CALLCONV pc_playlist_moved( sp_playlistcontainer *pc, sp_playlist *playlist, int position, int new_position, void *userdata );
    static void SP_CALLCONV pc_playlist_removed( sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata );
   QVariantList playlists(){ return m_playlists; }
   QVariantMap starred(){ return m_spStarredList; }
   sp_playlistcontainer* playlistcontainer() const { return m_spContainer; }
   void load();
   bool m_isLoaded;
   void init();

private:
   sp_session *m_session;
   sp_playlistcontainer *m_spContainer;
   QVariantMap m_spStarredList;
   sp_playlistcontainer_callbacks m_pcCallbacks;
   QVariantList m_playlists;


};
#endif // SPOTIFYPlaylist_H
