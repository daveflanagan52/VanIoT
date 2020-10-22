import React from 'react';
import { Map, Marker, TileLayer, Polyline } from 'react-leaflet';
import L from 'leaflet';
import Card from '../../Components/Card';

import './Location.scss';

export const vanIcon = new L.Icon({
    iconUrl: '../../Resources/van.png',
    iconAnchor: [16, 37],
    popupAnchor: [16, -37],
    iconSize: [32, 37],
});

class Location extends React.Component {
    render() {
        const { location } = this.props.data;
        const { locations } = this.props;
        return (
            <div className='location'>
                <h1>Location</h1>
                <Card>
                    <Map center={[location.latitude, location.longitude]} zoom={13} className='main-map' attributionControl={false}>
                        <TileLayer url="https://tiles.stadiamaps.com/tiles/alidade_smooth/{z}/{x}/{y}{r}.png" />
                        <Marker position={[location.latitude, location.longitude]} icon={vanIcon} />
                        <Polyline color="#639639" positions={locations.map(x => [x[0], x[1]])} />
                    </Map>
                </Card>
            </div>
        );
    }
};

export default Location;