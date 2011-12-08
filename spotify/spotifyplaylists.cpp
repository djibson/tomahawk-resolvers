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

#include "spotifyplaylists.h"
#include "callbacks.h"
#include <QVariant>



SpotifyPlaylist::SpotifyPlaylist(sp_session *session, QObject* parent )
    : QObject ( parent )
    , m_session ( session )
{
    qDebug() << "Starting a new SpotifyPlaylist!";
    load();
}

void
SpotifyPlaylist::load()
{

    m_isLoaded = false;
    m_spContainer = sp_session_playlistcontainer( m_session );

    m_pcCallbacks.playlist_added = &pc_playlist_added;
    m_pcCallbacks.playlist_removed = &pc_playlist_removed;
    m_pcCallbacks.container_loaded = &pc_loaded;
    m_pcCallbacks.playlist_moved = &pc_playlist_moved;

    for (int i = 0; i < sp_playlistcontainer_num_playlists(m_spContainer); i++) {
      sp_playlist_set_in_ram(m_session, sp_playlistcontainer_playlist(m_spContainer, i), true);
    }

    sp_playlistcontainer_add_callbacks( m_spContainer, &m_pcCallbacks, this );


}


/**
 * loadPlaylists thread
 */
void
*loadPlaylists( void *spotifyPlaylists )
{
  qDebug() << Q_FUNC_INFO;
  SpotifyPlaylist *sp_playlists = ( SpotifyPlaylist* )spotifyPlaylists;

  qDebug() << "starting load playlist thread";

   bool done = false;

   while (!done) {
     done = true;

     if (done == true)
       for (int i = 0; i < sp_playlistcontainer_num_playlists(sp_playlists->playlistcontainer()); i++) {
         sp_playlist_type spType = sp_playlistcontainer_playlist_type(sp_playlists->playlistcontainer(), i);
         if (spType != SP_PLAYLIST_TYPE_PLAYLIST)
           continue;

         if (!sp_playlist_is_loaded(sp_playlistcontainer_playlist(sp_playlists->playlistcontainer(), i))) {
           done = false;
           qDebug() << "All playlists not loaded";
           break;
         }
       }

     clock_t goal = 10000 + clock();
     while (goal > clock())
       ;
   }
   qDebug() << "init playliststore from loadthread";

}

void
SpotifyPlaylist::init() {

    if(!sp_playlistcontainer_is_loaded( m_spContainer ) )
    {
            qDebug() << "Container not loaded yet!";
            return;
    }

    QVariantList data;
    QStringList starred;
    m_playlists.clear();
    m_spStarredList.clear();
    qDebug() << "init playlist lookup";
    /// Add the playlists
    for ( int i = 0; i < sp_playlistcontainer_num_playlists( m_spContainer ); i++ )
    {

        sp_playlist_type spType = sp_playlistcontainer_playlist_type( m_spContainer, i );

        /// For now, iterate playlists, not folders.
        if ( spType != SP_PLAYLIST_TYPE_PLAYLIST )
            continue;

        int tracks = sp_playlist_num_tracks( sp_playlistcontainer_playlist( m_spContainer, i ) );
        /// Holders
        QVariantMap playlist;
        QStringList trackList;

        if ( tracks != 0 )
        {

           for ( int j = 0; j < tracks; j++ )
           {
                sp_track *t = sp_playlist_track( sp_playlistcontainer_playlist( m_spContainer, i ), j );
                /// Due to bug in QxTJSON Stringify, we need to escape dobbule qoutes
                QString track = QString( sp_track_name( t ) ).replace( "\"", "\'" ) + " - " + QString( sp_artist_name( sp_track_artist( t,0 ) ) ).replace( "\"", "\'" );
                /// Add to starredList

                if( sp_track_is_starred( m_session, t ) )
                {
                    qDebug() << sp_track_name(t) << "is starred";
                    if( !starred.contains( track ) )
                            starred << track;

                }

                /// Add to trackList
                trackList << track;            }
                playlist.insert( QString( sp_playlist_name( sp_playlistcontainer_playlist( m_spContainer, i ) ) ).replace( "\"", "\'" ), trackList );
                data << playlist;

            }
    }
    /// Append
    m_spStarredList.insert( "Starred tracks", starred );
    m_playlists = data;
    qDebug() << "Done";
}


void
SpotifyPlaylist::pc_loaded( sp_playlistcontainer *pc, void *userdata )
{
      qDebug() << "Rootlist synchronized";

      pthread_t initThread;
      if ( ( pthread_create( &initThread, NULL, &loadPlaylists, userdata) ) ) {
        qDebug() << "Failed to create playlist loader thread";
      }

}

void
SpotifyPlaylist::pc_playlist_added( sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata )
{
    Q_UNUSED(pc);
    Q_UNUSED(playlist);
    Q_UNUSED(position);
    Q_UNUSED(userdata);
    qDebug() << Q_FUNC_INFO;
    /*pthread_t initThread;
    if ( ( pthread_create( &initThread, NULL, &loadPlaylists, userdata) ) ) {
      qDebug() << "Failed to create playlist load thread";
    }*/
}

void
SpotifyPlaylist::pc_playlist_moved( sp_playlistcontainer *pc, sp_playlist *playlist, int position, int new_position, void *userdata )
{
    Q_UNUSED(pc);
    Q_UNUSED(playlist);
    Q_UNUSED(position);
    Q_UNUSED(new_position);
    Q_UNUSED(userdata);
    qDebug() << Q_FUNC_INFO;
    /*pthread_t initThread;
    if ( ( pthread_create( &initThread, NULL, &loadPlaylists, userdata) ) )
    {
      qDebug() << "Failed to create playlist load thread";
    }*/
}

void
SpotifyPlaylist::pc_playlist_removed( sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata )
{
    Q_UNUSED(pc);
    Q_UNUSED(playlist);
    Q_UNUSED(position);
    Q_UNUSED(userdata);
    qDebug() << Q_FUNC_INFO;
    /*pthread_t initThread;
    if ( ( pthread_create( &initThread, NULL, &loadPlaylists, userdata ) ) ){
      qDebug() << "Failed to create playlist load thread";
    }*/
}

